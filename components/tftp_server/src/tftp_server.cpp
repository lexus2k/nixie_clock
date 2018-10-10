#include "tftp_server.h"
#include <esp_log.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "sdkconfig.h"

static char* ip_to_str(struct sockaddr *addr)
{
    static char buf[INET_ADDRSTRLEN];
    return inet_ntop(AF_INET, addr, buf, INET_ADDRSTRLEN);
}

static char TAG[] = "TFTP";

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

// Max supported TFTP packet size
const int TFTP_DATA_SIZE = 512 + 4;

TFTP::TFTP(uint16_t port)
   : m_port( port )
{
}

TFTP::~TFTP()
{
}

void TFTP::process_write()
{
    ESP_LOGI(TAG, "receiving file: %s", m_filename.c_str());
    int handle = on_write( m_filename.c_str() );
    if (handle < 0)
    {
        ESP_LOGE(TAG, "Failed to open file for writing: %s: %s", tmpName.c_str(), strerror(errno));
        //return;
    }
    int total_size = 0;
    int next_block_num = 1;
    while (1)
    {
        socklen_t addr_len = sizeof(m_client);
        int size = recvfrom(m_sock, m_buffer, TFTP_DATA_SIZE, 0, &m_client, &addr_len);

        if (size < 0)
        {
            ESP_LOGE(TAG, "error on receive");
            break;
        }
        uint16_t code = ntohs(*(uint16_t*)(&m_buffer[0]));
        if ( code != TFTP_CMD_DATA )
        {
            ESP_LOGE(TAG, "not data packet received: [%d]", code);
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, m_buffer, size, ESP_LOG_INFO);
            if ( code != TFTP_CMD_WRQ )
            {
                break;
            }
            on_close();
            ESP_LOGI(TAG, "attempt to restart transmission?");

            parse_rq(size);
/*
            m_filename = std::string((char *)(m_buffer + 2));
            m_mode = std::string((char *)(m_buffer + 3 + m_filename.length())); */
            total_size = 0;
            next_block_num = 1;
//            send_ack(0);
            ESP_LOGI(TAG, "receiving file: %s", m_filename.c_str());
            handle = on_write( m_filename.c_str() );
            continue;
        }
        uint16_t block_num = ntohs(*(uint16_t*)(&m_buffer[2]));
        int data_size = size - 4;
        if ( block_num < next_block_num )
        {
            // Maybe this is dup, ignore it
            ESP_LOGI(TAG, "dup packet received: [%d], expected [%d]", block_num, next_block_num);
        }
        else
        {
            next_block_num++;
            total_size += data_size;
            on_write_data(&m_buffer[4], data_size);
            ESP_LOGD(TAG, "Block size: %d", data_size);
        }
        send_ack(block_num);
        if (size < TFTP_DATA_SIZE)
        {
            break;
        }
    }
    ESP_LOGI(TAG, "file received: %s (%d bytes)", m_filename.c_str(), total_size);
    on_close();
}

void TFTP::process_read()
{
    ESP_LOGD(TAG, "sending file: %s", m_filename.c_str());
    int file_size = on_read(m_filename.c_str());

    int block_num = 1;

    *(uint16_t*)(&m_buffer[0]) = htons(TFTP_CMD_DATA);

    while (file_size)
    {
        *(uint16_t*)(&m_buffer[2]) = htons(block_num);

        int size_read = on_read_data( &m_buffer[4], TFTP_DATA_SIZE - 4 );

        ESP_LOGD(TAG, "Sending data to %s, blockNumber=%d, size=%d",
                ip_to_str(&m_client), block_num, size_read);

        sendto( m_sock, m_buffer, size_read + 4, 0, &m_client, sizeof(m_client));

        if (size_read < TFTP_DATA_SIZE - 4)
        {
            break;
        } else
        {
            wait_for_ack(block_num);
        }
        block_num++;
    }
    ESP_LOGD(TAG, "File sent");
}

void TFTP::send_ack(uint16_t block_num)
{
    uint8_t data[4];

    *(uint16_t*)(&data[0]) = htons(TFTP_CMD_ACK);
    *(uint16_t*)(&data[2]) = htons(block_num);
    ESP_LOGD(TAG, "ack to %s, blockNumber=%d", ip_to_str(&m_client), block_num);
    sendto(m_sock, (uint8_t *)&data, sizeof(data), 0, &m_client, sizeof(struct sockaddr));
}


void TFTP::send_error(uint16_t code, const char *message)
{
    *(uint16_t *)(&m_buffer[0]) = htons(TFTP_CMD_ERROR);
    *(uint16_t *)(&m_buffer[2]) = htons(code);
    strcpy((char *)(&m_buffer[4]), message);
    sendto(m_sock, m_buffer, 4 + strlen(message) + 1, 0, &m_client, sizeof(m_client));
}

void TFTP::wait_for_ack(uint16_t block_num)
{
    uint8_t data[4];

    ESP_LOGD(TAG, "waiting for ack");
    socklen_t len = sizeof(m_client);
    int sizeRead = recvfrom(m_sock, (uint8_t *)&data, sizeof(data), 0, &m_client, &len);

    if ( (sizeRead != sizeof(data)) ||
         (ntohs(*(uint16_t *)(&data[0])) != TFTP_CMD_ACK) )
    {
        ESP_LOGE(TAG, "received wrong ack packet: %d", ntohs(*(uint16_t *)(&data[0])));
        send_error(ERROR_CODE_NOTDEFINED, "incorrect ack");
        return;
    }

    if (ntohs(*(uint16_t *)(&data[2])) != block_num)
    {
        ESP_LOGE(TAG, "received ack not in order");
        return;
    }
}

uint16_t TFTP::parse_rq(int full_size)
{
    uint8_t *ptr = m_buffer + 2;
    uint16_t cmd = ntohs(*(uint16_t*)(&m_buffer[0]));
    m_filename = std::string((char *)(ptr));
    ptr += strlen((char*)ptr) + 1;
    m_mode = std::string((char *)(ptr));
    ptr += strlen((char*)ptr) + 1;
    if (cmd == TFTP_CMD_WRQ)
    {
        if ( (ptr - m_buffer < full_size) && !strcmp((char *)ptr, "blksize") )
        {
           uint8_t data[] = { 6, 'b', 'l', 'k', 's', 'i', 'z', 'e', 0, '5', '1', '2', 0 };
           sendto(m_sock, data, sizeof(data), 0, &m_client, sizeof(m_client));
        }
        else
        {
            send_ack(0);
        }
    }
    return cmd;
}

int TFTP::start()
{
    if (m_sock >= 0)
    {
         return 0;
    }
    m_sock = socket( AF_INET, SOCK_DGRAM, 0);
    if (m_sock < 0)
    {
         return -1;
    }
    m_buffer = (uint8_t *)malloc(TFTP_DATA_SIZE);
    if ( m_buffer == nullptr )
    {
        close(m_sock);
        m_sock = -1;
        return -1;
    }

    int optval = 1;
    setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)m_port);
    if (bind( m_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "binding error");
        return 0;
    }

    ESP_LOGI(TAG, "Started on port %d", m_port);
    return 0;
}

int TFTP::run(bool wait_for)
{
    socklen_t len = sizeof(struct sockaddr);
    int result = recvfrom(m_sock, m_buffer, TFTP_DATA_SIZE, wait_for ? 0 : MSG_DONTWAIT, &m_client, &len);
    if (result < 0)
    {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
        {
             return 0;
        }
        return -1;
    }

    uint16_t cmd = parse_rq(result);
    switch(cmd)
    {
        case TFTP_CMD_WRQ:
            process_write();
            break;
        case TFTP_CMD_RRQ:
            process_read();
            break;
        default:
            ESP_LOGW(TAG, "unknown command %d", cmd);
            break;
    }
    return 0;
}

void TFTP::stop()
{
    if (m_sock >= 0)
    {
        close(m_sock);
        m_sock = -1;
        free(m_buffer);
    }
    m_buffer = nullptr;
}

int TFTP::on_read(const char *file)
{
    return -1;
}

int TFTP::on_write(const char *file)
{
    return -1;
}

int TFTP::on_read_data(uint8_t *buffer, int len)
{
    return -1;
}

int TFTP::on_write_data(uint8_t *buffer, int len)
{
    return -1;
}

void TFTP::on_close()
{
    return;
}


