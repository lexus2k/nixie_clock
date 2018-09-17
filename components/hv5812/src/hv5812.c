#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"


typedef struct
{
    int strobe;
    spi_device_handle_t handle;
} hv5812_data;


void hv5812_spi_bus_init(spi_host_device_t bus)
{
    spi_bus_config_t buscfg=
    {
        .miso_io_num= -1,
        .mosi_io_num= bus == VSPI_HOST ? 23 : 13,
        .sclk_io_num= bus == VSPI_HOST ? 18 : 14,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=32
    };
    spi_bus_initialize(bus, &buscfg, 0); // 0 -no dma
}

void hv5812_init(hv5812_data* handle, spi_host_device_t bus, int strobe, uint32_t freq)
{
    gpio_set_direction(strobe, GPIO_MODE_OUTPUT);
    gpio_set_level(strobe, 0);

    spi_device_interface_config_t devcfg=
    {
        .clock_speed_hz = 1000000,
        .mode=0,
        .spics_io_num = -1,
        .queue_size=7,       // max 7 transactions at a time
    };
    spi_bus_add_device(bus, &devcfg, &handle->handle);
    handle->strobe = strobe;
}

void hv5812_send(hv5812_data* handle, uint8_t *data, int len)
{
    while (len)
    {
        size_t sz = len > 32 ? 32: len;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length=8*sz;          // 8 bits
        t.tx_buffer = data;
        spi_device_transmit(handle->handle, &t);
        data+=sz;
        len-=sz;
    }
    // TODO: Add strobe here
}


void platform_spi_close(spi_host_device_t bus, hv5812_data* handle)
{
    // ... free all spi resources here
    spi_bus_remove_device( handle->handle );
    spi_bus_free( bus );
}

