#include "tftp_server.h"
#include <esp_log.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include <GeneralUtils.h>
//#include <string>
#include <stdio.h>
#include <errno.h>
#include <string.h>
//#include <Socket.h>

#include "sdkconfig.h"

static char* ip_to_str(struct sockaddr *addr)
{
    static char buf[INET_ADDRSTRLEN];
    return inet_ntop(AF_INET, addr, buf, INET_ADDRSTRLEN);
}

static char tag[] = "TFTP";

enum e_tftp_cmd
{
    TFTP_CMD_RRQ   = 1, // Read request
    TFTP_CMD_WRQ   = 2, // Write request
    TFTP_CMD_DATA  = 3, // Data
    TFTP_CMD_ACK   = 4, // Acknowledgement
    TFTP_CMD_ERROR = 5  // Error
};

enum ERRORCODE {
    ERROR_CODE_NOTDEFINED        = 0,
    ERROR_CODE_FILE_NOT_FOUND    = 1,
    ERROR_CODE_ACCESS_VIOLATION  = 2,
    ERROR_CODE_NO_SPACE          = 3,
    ERROR_CODE_ILLEGAL_OPERATION = 4,
    ERROR_CODE_UNKNOWN_ID        = 5,
    ERROR_CODE_FILE_EXISTS       = 6,
    ERROR_CODE_UNKNOWN_USER      = 7
};

/**
 * Size of the TFTP data payload.
 */
const int TFTP_DATA_SIZE=512;

struct data_packet {
    uint16_t blockNumber;
};



TFTP::TFTP() {
    m_baseDir = "";
    m_filename = "";
    m_mode     = "";
    m_opCode    = -1;
}

TFTP::~TFTP() {
}


/**
 * @brief Process a client read request.
 * @return N/A.
 */

void TFTP::processRRQ() {
/*
 *   2 bytes     2 bytes     n bytes
 *  ----------------------------------
 * | e_tftp_cmd |   Block #  |   Data     |
 *  ----------------------------------
 *
 */
    FILE *file;
    bool finished = false;

    ESP_LOGD(tag, "Reading TFTP data from file: %s", m_filename.c_str());
    std::string tmpName = m_baseDir + "/" + m_filename;
    /*
    struct stat buf;
    if (stat(tmpName.c_str(), &buf) != 0) {
        ESP_LOGE(tag, "Stat file: %s: %s", tmpName.c_str(), strerror(errno));
        return;
    }
    int length = buf.st_size;
    */

    int blockNumber = 1;

    file = fopen(tmpName.c_str(), "r");
    if (file == nullptr) {
        ESP_LOGE(tag, "Failed to open file for reading: %s: %s", tmpName.c_str(), strerror(errno));
        sendError(ERROR_CODE_FILE_NOT_FOUND, tmpName);
        return;
    }

    struct {
        uint16_t e_tftp_cmd;
        uint16_t blockNumber;
        uint8_t buf[TFTP_DATA_SIZE];
    } record;

    record.e_tftp_cmd = htons(TFTP_CMD_DATA); // Set the op code to be DATA.

    while(!finished) {

        record.blockNumber = htons(blockNumber);

        int sizeRead = fread(record.buf, 1, TFTP_DATA_SIZE, file);

        ESP_LOGD(tag, "Sending data to %s, blockNumber=%d, size=%d",
                ip_to_str(&m_partnerAddress), blockNumber, sizeRead);

                sendto( m_sock, (uint8_t*)&record, sizeRead+4, 0, &m_partnerAddress, sizeof(struct sockaddr));

        if (sizeRead < TFTP_DATA_SIZE) {
            finished = true;
        } else {
            waitForAck(blockNumber);
        }
        blockNumber++; // Increment the block number.
    }
    ESP_LOGD(tag, "File sent");
} // processRRQ


/**
 * @brief Process a client write request.
 * @return N/A.
 */
void TFTP::processWRQ() {
/*
 *        2 bytes    2 bytes       n bytes
 *        ---------------------------------
 * DATA  |  03   |   Block #  |    Data    |
 *        ---------------------------------
 * The e_tftp_cmd for data is 0x03 - TFTP_CMD_DATA
 */
    struct recv_data {
        uint16_t e_tftp_cmd;
        uint16_t blockNumber;
        uint8_t  data;
    } *pRecv_data;

    struct sockaddr recvAddr;
    uint8_t dataBuffer[TFTP_DATA_SIZE + 2 + 2];
    bool finished = false;

    FILE *file;

    ESP_LOGD(tag, "Writing TFTP data to file: %s", m_filename.c_str());
    std::string tmpName = m_baseDir + "/" + m_filename;
    file = fopen(tmpName.c_str(), "w");
    if (file == nullptr) {
        ESP_LOGE(tag, "Failed to open file for writing: %s: %s", tmpName.c_str(), strerror(errno));
        return;
    }
    while(!finished) {
        pRecv_data = (struct recv_data *)dataBuffer;
                socklen_t len = sizeof(struct sockaddr);
            int receivedSize = recvfrom(m_sock, dataBuffer, sizeof(dataBuffer), 0, &recvAddr, &len);

        if (receivedSize == -1) {
            ESP_LOGE(tag, "rc == -1 from receive_from");
        }
        struct data_packet dp;
        dp.blockNumber = ntohs(pRecv_data->blockNumber);
                fwrite((char *)&pRecv_data->data, receivedSize-4, 1, file);
        sendAck(dp.blockNumber);
        ESP_LOGD(tag, "Block size: %d", receivedSize-4);
        if (receivedSize-4 < TFTP_DATA_SIZE) {
            finished = true;
        }
    } // Finished
    fclose(file);
} // process


/**
 * @brief Send an acknowledgment back to the partner.
 * A TFTP acknowledgment packet contains an e_tftp_cmd (4) and a block number.
 *
 * @param [in] blockNumber The block number to send.
 * @return N/A.
 */
void TFTP::sendAck(uint16_t blockNumber) {
    struct {
        uint16_t e_tftp_cmd;
        uint16_t blockNumber;
    } ackData;

    ackData.e_tftp_cmd      = htons(TFTP_CMD_ACK);
    ackData.blockNumber = htons(blockNumber);

    ESP_LOGD(tag, "Sending ack to %s, blockNumber=%d", ip_to_str(&m_partnerAddress), blockNumber);
    sendto(m_sock, (uint8_t *)&ackData, sizeof(ackData), 0, &m_partnerAddress, sizeof(struct sockaddr));
} // sendAck


/**
 * @brief Start being a TFTP server.
 *
 * This function does not return.
 *
 * @param [in] port The port number on which to listen.  The default is 69.
 * @return N/A.
 */
void TFTP::start(uint16_t port) {
/*
 * Loop forever.  At the start of the loop we block waiting for an incoming client request.
 * The requests that we are expecting are either a request to read a file from the server
 * or write a file to the server.  Once we have received a request we then call the appropriate
 * handler to handle that type of request.  When the request has been completed, we start again.
 */
    ESP_LOGD(tag, "Starting TFTP::start() on port %d", port);
    m_sock = socket( AF_INET, SOCK_DGRAM, 0);
//        listen(server_sock, 0);
        int optval = 1;
        setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
//    serverSocket.listen(port, true); // Create a listening socket that is a datagram.
    while(true) {
        // This would be a good place to start a transaction in the background.
        uint16_t receivede_tftp_cmd = waitForRequest();
        switch(receivede_tftp_cmd) {
        // Handle the write request (client file upload)
            case e_tftp_cmd::TFTP_CMD_WRQ: {
                processWRQ();
                break;
            }

        // Handle the read request (server file download)
            case e_tftp_cmd::TFTP_CMD_RRQ: {
                processRRQ();
                break;
            }
        }
    } // End while loop
        close(m_sock);
        m_sock = -1;
} // run


/**
 * @brief Set the base dir for file access.
 * If we are asked to put a file to the file system, this is the base relative directory.
 * @param baseDir Base directory for file access.
 * @return N/A.
 */
void TFTP::setBaseDir(std::string baseDir) {
    m_baseDir = baseDir;
} // setBaseDir


/**
 * @brief Wait for an acknowledgment from the client.
 * After having sent data to the client, we expect an acknowledment back from the client.
 * This function causes us to wait for an incoming acknowledgment.
 */
void TFTP::waitForAck(uint16_t blockNumber) {
    struct {
        uint16_t e_tftp_cmd;
        uint16_t blockNumber;
    } ackData;

    ESP_LOGD(tag, "TFTP: Waiting for an acknowledgment request");
        socklen_t len = sizeof(struct sockaddr);
    int sizeRead = recvfrom(m_sock, (uint8_t *)&ackData, sizeof(ackData), 0, &m_partnerAddress, &len);
    ESP_LOGD(tag, "TFTP: Received some data.");

    if (sizeRead != sizeof(ackData)) {
        ESP_LOGE(tag, "waitForAck: Received %d but expected %d", sizeRead, sizeof(ackData));
        sendError(ERROR_CODE_NOTDEFINED, "Ack not correct size");
        return;
    }

    ackData.e_tftp_cmd      = ntohs(ackData.e_tftp_cmd);
    ackData.blockNumber = ntohs(ackData.blockNumber);

    if (ackData.e_tftp_cmd != e_tftp_cmd::TFTP_CMD_ACK) {
        ESP_LOGE(tag, "waitForAck: Received e_tftp_cmd %d but expected %d", ackData.e_tftp_cmd, e_tftp_cmd::TFTP_CMD_ACK);
        return;
    }

    if (ackData.blockNumber != blockNumber) {
        ESP_LOGE(tag, "waitForAck: Blocknumber received %d but expected %d", ackData.blockNumber, blockNumber);
        return;
    }
} // waitForAck


/**
 * @brief Wait for a client request.
 * A %TFTP server waits for requests to send or receive files.  A request can be
 * either WRQ (write request) which is a request from the client to write a new local
 * file or it can be a RRQ (read request) which is a request from the client to
 * read a local file.
 * @param pServerSocket The server socket on which to listen for client requests.
 * @return The op code received.
 */
uint16_t TFTP::waitForRequest() {
/*
 *        2 bytes    string   1 byte     string   1 byte
 *        -----------------------------------------------
 * RRQ/  | 01/02 |  Filename  |   0  |    Mode    |   0  |
 * WRQ    -----------------------------------------------
 */
    union {
        uint8_t buf[TFTP_DATA_SIZE];
        uint16_t e_tftp_cmd;
    } record;
    size_t length = 100;

    ESP_LOGD(tag, "TFTP: Waiting for a request");
        socklen_t len = sizeof(struct sockaddr);
    int result = recvfrom(m_sock, record.buf, length, 0, &m_partnerAddress, &len);
        if (result < 0)
        {
            return 0;
        }

    // Save the filename, mode and op code.

    m_filename = std::string((char *)(record.buf + 2));
    m_mode     = std::string((char *)(record.buf + 3 + m_filename.length()));
    m_opCode   = ntohs(record.e_tftp_cmd);
    switch(m_opCode) {

        // Handle the Write Request command.
        case TFTP_CMD_WRQ: {
//            m_partnerSocket.bind(0, INADDR_ANY);
            sendAck(0);
            break;
        }


        // Handle the Read request command.
        case TFTP_CMD_RRQ: {
             // m_partnerSocket.bind(0, INADDR_ANY);
            break;
        }

        default: {
            ESP_LOGD(tag, "Un-handled e_tftp_cmd: %d", m_opCode);
            break;
        }
    }
    return m_opCode;
} // waitForRequest

/**
 * @brief Send an error indication to the client.
 * @param [in] code Error code to send to the client.
 * @param [in] message Explanation message.
 * @return N/A.
 */
void TFTP::sendError(uint16_t code, std::string message) {
/*
 *  2 bytes     2 bytes      string    1 byte
 *  -----------------------------------------
 * | e_tftp_cmd |  ErrorCode |   ErrMsg   |   0  |
 *  -----------------------------------------
 */
    int size = 2  + 2 + message.length() + 1;
    uint8_t *buf = (uint8_t *)malloc(size);
    *(uint16_t *)(&buf[0]) = htons(e_tftp_cmd::TFTP_CMD_ERROR);
    *(uint16_t *)(&buf[2]) = htons(code);
    strcpy((char *)(&buf[4]), message.c_str());
    sendto(m_sock, buf, size, 0, &m_partnerAddress, sizeof(struct sockaddr));
    free(buf);
} // sendError
