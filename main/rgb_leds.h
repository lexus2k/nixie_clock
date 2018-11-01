#pragma once

#include "tlc59116.h"

class Tlc59116Leds
{
public:
    Tlc59116Leds(WireI2C& i2c);
    ~Tlc59116Leds();

    bool begin();
    void end();

    void enable( uint8_t index );
    void enable();

    void set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    void set_color(uint8_t r, uint8_t g, uint8_t b);

private:
    Tlc59116 m_chip[2];
    bool m_enabled[6]{};
};

