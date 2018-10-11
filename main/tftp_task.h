#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void start_tftp(void);
void stop_tftp(void);

#ifdef __cplusplus
}

#include "tftp_ota_server.h"

class TftpConfigServer: public TftpOtaServer
{
public:
    using TftpOtaServer::TftpOtaServer;

protected:
    int on_write(const char *file) override;
    int on_write_data(uint8_t *buffer, int len) override;
//    void on_close() override;

    int on_read(const char *file) override;
    int on_read_data(uint8_t *buffer, int len) override;
private:
    uint8_t m_is_nvram;
};

#endif

