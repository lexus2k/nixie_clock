#pragma once

#include "tlc59116.h"

class Tlc59116Leds
{
public:
    Tlc59116Leds(IWireI2C& i2c);
    ~Tlc59116Leds();

    bool begin();
    void end();

    void enable( uint8_t index );
    void enable();
    void disable( uint8_t index );
    void disable();
    void enable_blink();

    void set_min_pwm(uint8_t r, uint8_t g, uint8_t b);
    void set_max_pwm(uint8_t r, uint8_t g, uint8_t b);
    void set_brightness(uint8_t br);

    void set_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    void set_color(uint8_t r, uint8_t g, uint8_t b);
    void set_color(uint32_t color);
    void set_color(uint8_t index, uint32_t color);

private:
    Tlc59116 m_chip[2];
    bool m_enabled[6]{};
    uint8_t m_min[3] = {0,0,0};
    uint8_t m_max[3] = {255,255,255};

    uint8_t color_to_pwm(uint8_t index, uint8_t color);
    void update_led_out();
};

