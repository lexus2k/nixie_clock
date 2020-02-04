#pragma once

#include "spibus_intf.h"
#include "driver/spi_master.h"

class WireSPI: public IWireSPI
{
public:
    WireSPI(spi_host_device_t bus = VSPI_HOST);
    ~WireSPI();

    bool begin() override;
    void end() override;

    bool beginTransaction(uint32_t freq, int cs, int mode) override;
    void endTransaction() override;
    uint8_t transfer(uint8_t byte) override;
    int transfer( const uint8_t *data, int len) override;

private:
    spi_host_device_t m_bus;
    spi_device_handle_t m_handle;
};

