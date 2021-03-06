#include "hv5812.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32/rom/ets_sys.h"

Hv5812::Hv5812(IWireSPI& spi, gpio_num_t strobe)
    : m_spi(spi)
{
    setup(strobe);
}

Hv5812::Hv5812(IWireSPI& spi)
    : m_spi(spi)
{
}

Hv5812::~Hv5812()
{
}

void Hv5812::setup(gpio_num_t strobe)
{
    m_strobe = strobe;
    gpio_set_direction(m_strobe, GPIO_MODE_OUTPUT);
    gpio_set_level(m_strobe, 0);
}

bool Hv5812::begin()
{
    return true;
}

void Hv5812::end()
{
}


bool Hv5812::write(const uint8_t *data, int len)
{
    m_spi.beginTransaction( 1000000, -1, 0);
    m_spi.transfer(data, len);
    m_spi.endTransaction();
    ets_delay_us(1);
    gpio_set_level(m_strobe, 1);
    ets_delay_us(1);
    gpio_set_level(m_strobe, 0);
    return true;
}

