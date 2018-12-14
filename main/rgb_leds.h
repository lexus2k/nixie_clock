#pragma once

#include "tlc59116.h"
#include <vector>

typedef struct
{
    uint8_t chip_index;
    uint8_t channel_index;
} led_channel_info_t;

typedef struct
{
    led_channel_info_t red;
    led_channel_info_t green;
    led_channel_info_t blue;
} rgd_led_info_t;

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
    std::vector<rgd_led_info_t> m_leds;
    std::vector<bool> m_enabled;
    uint8_t m_min[3] = {0,0,0};
    uint8_t m_max[3] = {255,255,255};

    uint8_t color_to_pwm(uint8_t index, uint8_t color);
    void update_led_out();
};

