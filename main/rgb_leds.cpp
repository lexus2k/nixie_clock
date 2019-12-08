#include "rgb_leds.h"
#include "platform/system.h"
#include <stdint.h>
#include <string.h>
#include "esp_log.h"

//static const char *TAG = "TFTP";

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
    m_color.clear();
    for(int i=0; i<i2c_addresses.size(); i++)
    {
        m_chips.emplace_back(Tlc59116( m_i2c, i2c_addresses[i] ));
    }
    m_leds = leds;
    m_enabled.resize( m_leds.size(), false );
    m_color.resize( m_leds.size(), false );
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
    m_timer = micros();
    return true;
}

void Tlc59116Leds::end()
{
    for (auto &chip: m_chips)
    {
        chip.end();
    }
}

void Tlc59116Leds::update()
{
    static const uint32_t UPDATE_STEP = 50000; // 50 milliseconds
    uint32_t ts = micros();
    while ( static_cast<uint32_t>(ts - m_timer) >= UPDATE_STEP )
    {
        m_timer += UPDATE_STEP;
        switch (m_mode)
        {
            case LedsMode::RAINBOW:
            {
                if (!m_modeArg2) m_modeArg1 = 0x00F80000;
                if ( m_modeArg1 <= 0x00FFFFFF || m_modeArg1 >= 0x3E000000 )
                {
                    if ( ++m_modeArg2 > 6 ) m_modeArg2 = 0;
                }
                if ( m_modeArg2 == 1 ) { m_modeArg1 += 0x01000400; m_modeArg1 -= 0x00040000; }
                if ( m_modeArg2 == 2 ) { m_modeArg1 += 0x00000004; m_modeArg1 -= 0x01000400; }
                if ( m_modeArg2 == 3 ) { m_modeArg1 += 0x01040000; m_modeArg1 -= 0x00000004; }
                if ( m_modeArg2 == 4 ) { m_modeArg1 += 0x00000400; m_modeArg1 -= 0x01040000; }
                if ( m_modeArg2 == 5 ) { m_modeArg1 += 0x01000004; m_modeArg1 -= 0x00000400; }
                if ( m_modeArg2 == 6 ) { m_modeArg1 += 0x00040000; m_modeArg1 -= 0x01000004; }
                set_color_internal( m_modeArg1 );
                break;
            }
            case LedsMode::RAINBOW_2:
            {
                if (!m_modeArg2) { m_modeArg1 = 0x00F80000; }
                if ( m_modeArg1 <= 0x00FFFFFF || m_modeArg1 >= 0x1E000000 )
                {
                    if ( ++m_modeArg2 > 6 ) m_modeArg2 = 0;
                }
                if ( m_modeArg2 == 1 ) { m_modeArg1 += 0x01000800; m_modeArg1 -= 0x00080000; }
                if ( m_modeArg2 == 2 ) { m_modeArg1 += 0x00000008; m_modeArg1 -= 0x01000800; }
                if ( m_modeArg2 == 3 ) { m_modeArg1 += 0x01080000; m_modeArg1 -= 0x00000008; }
                if ( m_modeArg2 == 4 ) { m_modeArg1 += 0x00000800; m_modeArg1 -= 0x01080000; }
                if ( m_modeArg2 == 5 ) { m_modeArg1 += 0x01000008; m_modeArg1 -= 0x00000800; }
                if ( m_modeArg2 == 6 ) { m_modeArg1 += 0x00080000; m_modeArg1 -= 0x01000008; }
                for (int i=0; i < m_leds.size(); i++)
                {
                    uint32_t color = (i & 1 ? m_modeArg1: m_modeArg1) & 0x00FFFFFF;
                    set_color_internal(i, (color >> ((i & 0x3) * 8)) | (color << ((3 - (i & 0x03))*8)) );
                }
                break;
            }
            case LedsMode::NEW_YEAR:
            {
                m_modeArg2 += UPDATE_STEP;
                if ( m_modeArg2 >= 300000 )
                {
                    m_modeArg2 = 0;
                    const uint32_t colors[] = { 0x00FF0000, 0x0000FF00, 0x00FFFF00, 0x000000FF, 0x00FF0000, 0x0000FFFF };
                    if ( ++m_modeArg1 > 1 ) m_modeArg1 = 0;
                    for (int i=0; i < m_leds.size(); i++)
                    {
                        set_color_internal(i, ((i + m_modeArg1) & 1) ? colors[i % 6] : 0x00000000 );
                    }
                }
                break;
            }
            case LedsMode::BLINK:
            {
                m_modeArg2 += UPDATE_STEP;
                if ( m_modeArg2 >= 500000 )
                {
                    m_modeArg2 = 0;
                    m_modeArg1 = !m_modeArg1;
                    m_modeArg1 ? set_color_internal( 0x00000000 ) : reload_color_internal();
                }
                break;
            }
            case LedsMode::NORMAL: break;
            default: break;
        }
    }
}

void Tlc59116Leds::set_mode( LedsMode mode )
{
    m_mode = mode;
    m_modeArg1 = 0;
    m_modeArg2 = 0;
    m_modeArg3 = 0;
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
        chip.set_blinking( 0x30 );
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
    set_color( index, (r<<16) | (g<<8) | b );
}

void Tlc59116Leds::set_color(uint8_t r, uint8_t g, uint8_t b)
{
    set_color((r << 16) | (g << 8) | b);
}

void Tlc59116Leds::set_color(uint32_t color)
{
    for (int i=0; i<m_leds.size(); i++)
    {
        set_color(i, color );
    }
}

void Tlc59116Leds::set_color(uint8_t index, uint32_t color)
{
    if ( index >= m_leds.size() )
    {
        return;
    }
    m_color[index] = color;
    if ( m_mode == LedsMode::NORMAL )
    {
        set_color_internal( index, color );
    }
}

void Tlc59116Leds::set_color_internal(uint32_t color)
{
    for (int i=0; i<m_leds.size(); i++)
    {
        set_color_internal(i, color );
    }
}

void Tlc59116Leds::set_color_internal(uint8_t index, uint32_t color)
{
    if ( index >= m_leds.size() )
    {
        return;
    }
    m_chips[ m_leds[index].red.chip_index ].set_brightness(
        m_leds[index].red.channel_index, color_to_pwm(0, (color >> 16) & 0xFF) );
    m_chips[ m_leds[index].green.chip_index ].set_brightness(
        m_leds[index].green.channel_index, color_to_pwm(0, (color >> 8) & 0xFF) );
    m_chips[ m_leds[index].blue.chip_index ].set_brightness(
        m_leds[index].blue.channel_index, color_to_pwm(0, color & 0xFF) );
}

void Tlc59116Leds::reload_color_internal()
{
    for (int i=0; i<m_leds.size(); i++)
    {
        set_color_internal(i, m_color[i] );
    }
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
