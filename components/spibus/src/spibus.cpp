#include "spibus.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"

WireSPI::WireSPI(spi_host_device_t bus)
    : m_bus( bus )
{
}

WireSPI::~WireSPI()
{
}

bool WireSPI::begin()
{
    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.miso_io_num= -1;
    buscfg.mosi_io_num= (m_bus == VSPI_HOST) ? 23 : 13;
    buscfg.sclk_io_num= (m_bus == VSPI_HOST) ? 18 : 14;
    buscfg.quadwp_io_num= -1;
    buscfg.quadhd_io_num= -1;
    buscfg.max_transfer_sz= 32;
    spi_bus_initialize(m_bus, &buscfg, 0); // 0 -no dma
    return true;
}

void WireSPI::end()
{
    spi_bus_free( m_bus );
}

bool WireSPI::beginTransaction(uint32_t freq, int cs, int mode)
{
    spi_device_interface_config_t devcfg;
    memset(&devcfg, 0, sizeof(devcfg));
    devcfg.clock_speed_hz = freq;
    devcfg.mode=0;
    devcfg.spics_io_num = cs;
    devcfg.queue_size=7;       // max 7 transactions at a time
    spi_bus_add_device(m_bus, &devcfg, &m_handle);
    return true;
}

void WireSPI::endTransaction()
{
    spi_bus_remove_device( m_handle );
}

uint8_t WireSPI::transfer(uint8_t byte)
{
    transfer(&byte, 1);
    return 0;
}

int WireSPI::transfer( const uint8_t *data, int len)
{
    while (len)
    {
        size_t sz = len > 32 ? 32: len;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length=8*sz;          // 8 bits
        t.tx_buffer = data;
        spi_device_transmit(m_handle, &t);
        data+=sz;
        len-=sz;
    }
    return 0;
}
