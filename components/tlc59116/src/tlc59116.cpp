#include "tlc59116.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

Tlc59116::Tlc59116(IWireI2C& i2c, uint8_t address)
    : m_i2c(i2c)
    , m_address(address)
{
}

Tlc59116::~Tlc59116()
{
}

bool Tlc59116::begin()
{
    m_i2c.beginTransmission(m_address);
    m_i2c.write(0x80); // autoincrement
    m_i2c.write(0x00); // disable all call address immediately
    m_detected = m_i2c.endTransmission() >= 0;
    set_mode( TLC59116_MODE_GROUP_DIMMING );
    for(int i=0; i<16; i++)
    {
        set_brightness( i, 0x00 );
    }
    set_brightness( 0x00 );
    set_blinking(0x0);
    update_leds( m_enabled_leds );
    return true;
}

void Tlc59116::end()
{
}

void Tlc59116::on(uint16_t leds)
{
    m_enabled_leds |= leds;
    update_leds( m_enabled_leds );
}

void Tlc59116::off(uint16_t leds)
{
    m_enabled_leds &= ~leds;
    update_leds( m_enabled_leds );
}

void Tlc59116::off()
{
    m_enabled_leds = 0;
    update_leds( m_enabled_leds );
}

void Tlc59116::set(uint16_t leds)
{
    m_enabled_leds = leds;
    update_leds( m_enabled_leds );
}

void Tlc59116::set_brightness(uint8_t br)
{
    if ( !m_detected ) return;
    m_i2c.beginTransmission(m_address);
    // Write to the GRPPWM register
    m_i2c.write(0x12);
    m_i2c.write(br);
    m_i2c.endTransmission();
}

void Tlc59116::set_blinking(uint8_t blinking)
{
    if ( !m_detected ) return;
    m_i2c.beginTransmission(m_address);
    // Write to the GRPFREQ register
    m_i2c.write(0x13);
    m_i2c.write(blinking);
    m_i2c.endTransmission();
}

void Tlc59116::set_mode(uint8_t mode)
{
    if ( !m_detected ) return;
    m_i2c.beginTransmission(m_address);
    // Write to the MODE2 register
    m_i2c.write(0x01);
    m_i2c.write(mode);
    m_i2c.endTransmission();
}

void Tlc59116::set_brightness(uint8_t led, uint8_t brightness)
{
    if ( !m_detected ) return;
    m_i2c.beginTransmission(m_address);
    // Write to the GRPPWM register
    m_i2c.write(0x02 + led);
    m_i2c.write(brightness);
    m_i2c.endTransmission();
}

void Tlc59116::update_leds(uint16_t leds)
{
    uint8_t val = 0;
    const uint8_t led_mode = (1<<1) | (1<<0);
    if ( !m_detected ) return;
    m_i2c.beginTransmission( m_address );
    // Write to consecutive registers, starting with LEDOUT0
    m_i2c.write( 0x80 + 0x14 );
    for (int i=0; i< 16; i++)
    {
        if (leds & 0x01)
        {
            val |= (led_mode << ((i & 0x03) * 2));
        }
        leds >>= 1;
        if ( (i & 0x03) == 0x03)
        {
            m_i2c.write(val);
            val = 0;
        }
    }
    m_i2c.endTransmission();
}

