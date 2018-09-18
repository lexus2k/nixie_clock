#pragma once

#include <stdint.h>
#include "driver/spi_master.h"

class WireSPI
{
public:
    WireSPI(spi_host_device_t bus = VSPI_HOST);
    ~WireSPI();

    bool begin();
    void end();

    bool beginTransaction(uint32_t freq, int cs, int mode);
    void endTransaction();
    uint8_t transfer(uint8_t byte);
    int transfer( const uint8_t *data, int len);

private:
    spi_host_device_t m_bus;
    spi_device_handle_t m_handle;
};

