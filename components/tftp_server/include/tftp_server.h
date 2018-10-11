#pragma once

#define TFTP_DEFAULT_PORT (69)

#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>


class TFTP
{
public:
    TFTP(uint16_t port = TFTP_DEFAULT_PORT);
    ~TFTP();

    int start();
    int run(bool wait_for = true);
    void stop();

protected:
    void send_ack(uint16_t block_num);
    void send_error(uint16_t code, const char *message);
    int wait_for_ack(uint16_t block_num);


    virtual int on_read(const char *file);
    virtual int on_write(const char *file);
    virtual int on_read_data(uint8_t *buffer, int len);
    virtual int on_write_data(uint8_t *buffer, int len);
    virtual void on_close();

private:
    uint16_t m_port;
    int m_sock = -1;
    struct sockaddr m_client;
    uint8_t *m_buffer = nullptr;
    int m_data_size;

    int process_write();
    int process_read();
    int parse_wrq();
    int parse_rrq();
    int parse_rq();
};

