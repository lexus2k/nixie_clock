#include "rgb_leds.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

Tlc59116Leds::Tlc59116Leds(IWireI2C& i2c)
   : m_i2c( i2c )
   , m_chips(0, Tlc59116(i2c, 0b1100000))
   , m_leds(0)
{
}


Tlc59116Leds::~Tlc59116Leds()
{
}

void Tlc59116Leds::setup(const std::vector<uint8_t> &i2c_addresses,
                         const std::vector<rgd_led_info_t> &leds)
{
    m_chips.clear();
    m_enabled.clear();
    for(int i=0; i<i2c_addresses.size(); i++)
    {
        m_chips.emplace_back(Tlc59116( m_i2c, i2c_addresses[i] ));
    }
    m_leds = leds;
    m_enabled.resize( m_leds.size(), false );
}

bool Tlc59116Leds::begin()
{
    for (auto &chip: m_chips)
    {
        chip.begin();
    }
    for (int i=0; i < m_leds.size(); i++)
    {
        set_color(i, 0, 0, 0);
    }
    return true;
}

void Tlc59116Leds::end()
{
    for (auto &chip: m_chips)
    {
        chip.end();
    }
}

void Tlc59116Leds::set_brightness(uint8_t br)
{
    for (auto &chip: m_chips)
    {
        chip.set_brightness( br );
    }
}

void Tlc59116Leds::enable_blink()
{
    for (auto &chip: m_chips)
    {
        chip.set_mode( TLC59116_MODE_GROUP_BLINKING );
        chip.set_blinking( 0x20 );
    }
}

void Tlc59116Leds::enable( uint8_t index )
{
    if ( index >= m_leds.size() )
    {
        return;
    }
    m_enabled[index] = true;
    update_led_out();
}

void Tlc59116Leds::disable( uint8_t index )
{
    if ( index >= m_leds.size() )
    {
        return;
    }
    m_enabled[index] = false;
    update_led_out();
}

void Tlc59116Leds::enable()
{
    for (int i=0; i<m_leds.size(); i++)
    {
        enable( i );
    }
}

void Tlc59116Leds::disable()
{
    for (int i=0; i<m_leds.size(); i++)
    {
        disable( i );
    }
}

void Tlc59116Leds::set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if ( index >= m_leds.size() )
    {
        return;
    }
    m_chips[ m_leds[index].red.chip_index ].set_brightness( m_leds[index].red.channel_index, color_to_pwm(0, r) );
    m_chips[ m_leds[index].green.chip_index ].set_brightness( m_leds[index].green.channel_index, color_to_pwm(0, g) );
    m_chips[ m_leds[index].blue.chip_index ].set_brightness( m_leds[index].blue.channel_index, color_to_pwm(0, b) );
}

void Tlc59116Leds::set_color(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i=0; i<m_leds.size(); i++)
    {
        set_color(i,r,g,b );
    }
}

void Tlc59116Leds::set_color(uint32_t color)
{
    set_color( color >> 16, color >> 8, color );
}

void Tlc59116Leds::set_color(uint8_t index, uint32_t color)
{
    set_color( index, color >> 16, color >> 8, color );
}

void Tlc59116Leds::set_min_pwm(uint8_t r, uint8_t g, uint8_t b)
{
    m_min[0] = r;
    m_min[1] = g;
    m_min[2] = b;
}

void Tlc59116Leds::set_max_pwm(uint8_t r, uint8_t g, uint8_t b)
{
    m_max[0] = r;
    m_max[1] = g;
    m_max[2] = b;
}

uint8_t Tlc59116Leds::color_to_pwm(uint8_t index, uint8_t color)
{
    return m_min[index] + (uint16_t)color * (uint16_t)(m_max[index] - m_min[index]) / 255;
}

void Tlc59116Leds::update_led_out()
{
    std::vector<uint16_t> pins(m_chips.size(), 0);
    for (int i=0; i<m_leds.size(); i++)
    {
        if ( m_enabled[i] )
        {
            pins[ m_leds[i].red.chip_index ] |= (1 << m_leds[i].red.channel_index);
            pins[ m_leds[i].green.chip_index ] |= (1 << m_leds[i].green.channel_index);
            pins[ m_leds[i].blue.chip_index ] |= (1 << m_leds[i].blue.channel_index);
        }
    }
    for (int i=0; i<m_chips.size(); i++)
    {
        m_chips[i].set( pins[i] );
    }
}
