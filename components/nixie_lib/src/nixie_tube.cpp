#include "nixie_tube.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void NixieTube::begin()
{
    if (m_pin >= 0)
    {
        gpio_set_direction(static_cast<gpio_num_t>(m_pin), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::end()
{
    if (m_pin >= 0)
    {
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::set(int digit)
{
    if (m_enabled)
    {
        m_pinmux.clear(m_index, m_digit);
        m_digit = digit;
        m_pinmux.set(m_index, m_digit);
    }
}

void NixieTube::off()
{
    m_pinmux.clear(m_index, m_digit);
    m_enabled = false;
}

void NixieTube::on()
{
    m_pinmux.set(m_index, m_digit);
    m_enabled = true;
}

void NixieTube::update()
{
}

