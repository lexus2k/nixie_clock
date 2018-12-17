#include "hv5812_group_controller.h"
#include <algorithm>

PinGroupControllerHv5812::PinGroupControllerHv5812(WireSPI& spi)
    : m_hv5812(spi)
{
}

PinGroupControllerHv5812::PinGroupControllerHv5812(WireSPI& spi, gpio_num_t strobe)
    : m_hv5812(spi)
{
    setup(strobe);
}

void PinGroupControllerHv5812::setup(gpio_num_t strobe, const std::vector<int>& pin_map)
{
    m_hv5812.setup( strobe );
    set_map(pin_map);
}


void PinGroupControllerHv5812::set_map(const std::vector<int>& pin_map)
{
    m_map = pin_map;
    auto max_element = std::max_element(m_map.begin(), m_map.end());
    int pin_count = max_element == m_map.end() ? 0 : (*max_element + 1);
    m_data.resize(( pin_count + 7 ) >> 3, 0xFF);
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
    m_hv5812.write(m_data.data(), m_data.size());
}

void PinGroupControllerHv5812::set(int pin)
{
    if (pin >= m_map.size())
    {
        return;
    }
    int mapped = m_map[pin];
    if ( mapped >= 0)
    {
        m_data[ m_data.size() - 1 - mapped/8 ] &= ~(1 << (mapped & 0x07));
    }
}

void PinGroupControllerHv5812::clear(int pin)
{
    if (pin >= m_map.size())
    {
        return;
    }
    int mapped = m_map[pin];
    if ( mapped >= 0)
    {
        m_data[ m_data.size() - 1 - mapped/8] |= (1 << (mapped & 0x07));
    }
}

