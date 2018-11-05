#include "pin_muxers.h"

PinMuxHv5812::PinMuxHv5812(WireSPI& spi, gpio_num_t strobe, int hv5812_count)
    : m_hv5812(spi, strobe)
    , m_hv5812_count(hv5812_count)
{
    memset( m_data, 0xFF, sizeof(m_data) );
}

void PinMuxHv5812::set_map(const int* pinMap, int size, int pinsPerModule )
{
    m_map = pinMap;
    m_size_bits = size;
    m_module_pins = pinsPerModule;
}

void PinMuxHv5812::begin()
{
    m_hv5812.begin();
}

void PinMuxHv5812::end()
{
    m_hv5812.end();
}

void PinMuxHv5812::update()
{
    m_hv5812.write(m_data, dataLen());
}

void PinMuxHv5812::set(int n, int pin)
{
    int index = n * m_module_pins + pin;
    if (index >= m_size_bits)
    {
        return;
    }
    int mapped = m_map[index];
    if ( mapped >= 0)
    {
        m_data[ dataLen() - 1 - mapped/8] &= ~(1 << (mapped & 0x07));
    }
}

void PinMuxHv5812::clear(int n, int pin)
{
    int index = n * m_module_pins + pin;
    if (index >= m_size_bits)
    {
        return;
    }
    int mapped = m_map[index];
    if ( mapped >= 0)
    {
        m_data[ dataLen() - 1 - mapped/8] |= (1 << (mapped & 0x07));
    }
}

