#include "rgb_leds.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

Tlc59116Leds::Tlc59116Leds(IWireI2C& i2c)
    : m_chip{ Tlc59116( i2c, 0b1100000 ), Tlc59116( i2c, 0b1100001 ) }
{
}


Tlc59116Leds::~Tlc59116Leds()
{
}

bool Tlc59116Leds::begin()
{
    m_chip[0].begin();
    m_chip[1].begin();
    for (int i=0; i<6; i++)
    {
        set_color(i, 0, 0, 0);
    }
    return true;
}

void Tlc59116Leds::end()
{
    m_chip[0].end();
    m_chip[1].end();
}

void Tlc59116Leds::set_brightness(uint8_t br)
{
    m_chip[0].set_brightness( br );
    m_chip[1].set_brightness( br );
}

void Tlc59116Leds::enable( uint8_t index )
{
    if ( index > 5 )
    {
        return;
    }
    m_enabled[index] = true;
    update_led_out();
}

void Tlc59116Leds::disable( uint8_t index )
{
    if ( index > 5 )
    {
        return;
    }
    m_enabled[index] = false;
    update_led_out();
}

void Tlc59116Leds::enable()
{
    for (int i=0; i<6; i++)
    {
        enable( i );
    }
}

void Tlc59116Leds::disable()
{
    for (int i=0; i<6; i++)
    {
        disable( i );
    }
}

void Tlc59116Leds::set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if ( index > 5 )
    {
        return;
    }
    uint16_t base_index = (index % 3) * 3;
    m_chip[index/3].set_brightness(base_index + 0, color_to_pwm(0, r) );
    m_chip[index/3].set_brightness(base_index + 1, color_to_pwm(1, g) );
    m_chip[index/3].set_brightness(base_index + 2, color_to_pwm(2, b) );
}

void Tlc59116Leds::set_color(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i=0; i<6; i++)
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
    uint16_t pins = 0x07;
    m_chip[0].set( ( m_enabled[0] ? (pins << 0) : 0 ) |
                   ( m_enabled[1] ? (pins << 3) : 0 ) |
                   ( m_enabled[2] ? (pins << 6) : 0 ) );
    m_chip[1].set( ( m_enabled[3] ? (pins << 0) : 0 ) |
                   ( m_enabled[4] ? (pins << 3) : 0 ) |
                   ( m_enabled[5] ? (pins << 6) : 0 ) );
/*    m_chip[0].set( 0xFFFF );
    m_chip[1].set( 0xFFFF ); */
}
