#pragma once

#include "tftp_server.h"
#include <esp_ota_ops.h>
#include <nvs_flash.h>

class TftpOtaServer: public TFTP
{
public:
    using TFTP::TFTP;

protected:
    int on_write(const char *file) override;
    int on_write_data(uint8_t *buffer, int len) override;
    void on_close() override;

private:
    esp_ota_handle_t m_ota_handle = 0;
    const esp_partition_t* m_next_partition = nullptr;
};

