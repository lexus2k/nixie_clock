#include "hv5812_group_controller.h"

PinGroupControllerHv5812::PinGroupControllerHv5812(WireSPI& spi)
    : m_hv5812(spi)
{
}

PinGroupControllerHv5812::PinGroupControllerHv5812(WireSPI& spi, gpio_num_t strobe, int hv5812_count)
    : m_hv5812(spi)
{
    setup(strobe, hv5812_count);
}

void PinGroupControllerHv5812::setup(gpio_num_t strobe, int hv5812_count)
{
    m_hv5812.setup( strobe );
    m_hv5812_count = hv5812_count;
    memset( m_data, 0xFF, sizeof(m_data) );
}


void PinGroupControllerHv5812::set_map(const int* pinMap, int size)
{
    m_map = pinMap;
    m_size_bits = size;
}

void PinGroupControllerHv5812::begin()
{
    m_hv5812.begin();
    update();
}

void PinGroupControllerHv5812::end()
{
    m_hv5812.end();
}

void PinGroupControllerHv5812::update()
{
    m_hv5812.write(m_data, dataLen());
}

void PinGroupControllerHv5812::set(int pin)
{
    if (pin >= m_size_bits)
    {
        return;
    }
    int mapped = m_map[pin];
    if ( mapped >= 0)
    {
        m_data[ dataLen() - 1 - mapped/8 ] &= ~(1 << (mapped & 0x07));
    }
}

void PinGroupControllerHv5812::clear(int pin)
{
    if (pin >= m_size_bits)
    {
        return;
    }
    int mapped = m_map[pin];
    if ( mapped >= 0)
    {
        m_data[ dataLen() - 1 - mapped/8] |= (1 << (mapped & 0x07));
    }
}

