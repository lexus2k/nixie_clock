#include "rgb_leds.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

Tlc59116Leds::Tlc59116Leds(WireI2C& i2c)
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

void Tlc59116Leds::enable( uint8_t index )
{
    if ( index > 5 )
    {
        return;
    }
    m_enabled[index] = true;
    uint16_t pins = 0x07;
/*    m_chip[0].enable_leds( m_enabled[0] ? (pins << 0) : 0 |
                           m_enabled[1] ? (pins << 3) : 0 |
                           m_enabled[2] ? (pins << 6) : 0 );
    m_chip[1].enable_leds( m_enabled[3] ? (pins << 0) : 0 |
                           m_enabled[4] ? (pins << 3) : 0 |
                           m_enabled[5] ? (pins << 6) : 0 ); */
    m_chip[0].enable_leds( 0xFFFF );
    m_chip[1].enable_leds( 0xFFFF );
}

void Tlc59116Leds::enable()
{
    for (int i=0; i<6; i++)
    {
        enable( i );
    }
}

void Tlc59116Leds::set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if ( index > 5 )
    {
        return;
    }
    uint16_t base_index = (index % 3) * 3;
    m_chip[index/3].set_brightness(base_index + 0, r);
    m_chip[index/3].set_brightness(base_index + 1, g);
    m_chip[index/3].set_brightness(base_index + 2, b);
}




