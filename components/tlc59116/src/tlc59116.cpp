#include "tlc59116.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

Tlc59116::Tlc59116(WireI2C& i2c, uint8_t address)
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
    m_i2c.write(0x00);
    for(int j=0; j<16; j++)
    {
        m_i2c.write(0xFF);
    }
    m_i2c.write(0xFF); // group
    m_i2c.write(0x0); // not blinking
    for(int j=0; j<4; j++)
    {
        m_i2c.write(0x0);
    }
    m_i2c.endTransmission();
    return true;
}

void Tlc59116::enable_leds(uint16_t leds)
{
    uint8_t registerVal=0;
    uint8_t registerIncrement = 0b11;
    // Write the value to the LEDs
    m_i2c.beginTransmission(m_address);
    // Write to consecutive registers, starting with LEDOUT0
    m_i2c.write(0x80 + 0x14);
    // Write the value for LEDs
    for (int i=0; i< 16; i++)
    {
        if (leds & 0x01)
            registerVal += registerIncrement;
        // Move to the next LED
        leds >>= 1;
        // Are 4 LED values in the register now?
        if (registerIncrement == 0b11000000)
        {
            // The register can be written out now
            m_i2c.write(registerVal);
            registerVal = 0;
            registerIncrement = 0b11;
        }
        else
        {
            // Move to the next increment
            registerIncrement <<= 2;
        }
    }
    m_i2c.endTransmission();
}

void Tlc59116::set_brightness(uint8_t br)
{
    m_i2c.beginTransmission(m_address);
    // Write to the GRPPWM register
    m_i2c.write(0x12);
    m_i2c.write(br);
    m_i2c.endTransmission();
}

void Tlc59116::set_brightness(uint8_t led, uint8_t brightness)
{
    m_i2c.beginTransmission(m_address);
    // Write to the GRPPWM register
    m_i2c.write(0x02 + led);
    m_i2c.write(brightness);
    m_i2c.endTransmission();
}

void Tlc59116::end()
{
}
