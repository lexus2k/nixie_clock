#pragma once

#include "pin_muxers.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieTube
{
public:
    NixieTube(int index, PinMux* mux = nullptr)
        : m_index(index)
        , m_pinmux(mux)
        , m_pin(-1) {};
    ~NixieTube() = default;

    void set_anod(gpio_num_t pin);

    void set_pin_muxer(PinMux* muxer)
    {
        m_pinmux = muxer;
    }

    void set(int digit);
    void on();
    void off();
    void set_brightness(uint8_t brightness);
    void set_smooth_brightness(uint8_t brightness);
    void scroll(int value);
    void overlap(int value);

    void begin();
    void end();
    void update();

    void enable_pwm(ledc_channel_t channel, ledc_timer_t timer = LEDC_TIMER_0);
    static void init_ledc_timer(ledc_timer_t timer = LEDC_TIMER_0, ledc_mode_t mode = LEDC_HIGH_SPEED_MODE);
    static void enable_hw_fade();
    static void disable_hw_fade();

protected:
    int m_index = 0;
    PinMux* m_pinmux = nullptr;
    int m_pin = -1;
    bool m_enabled = false;
    int m_value = 0;
    int m_target_value = 0;
    bool m_pwmMode = false;
    ledc_channel_t m_channel;
    uint8_t m_brightness = 0;
    uint8_t m_target_brightness = 0;
    uint8_t m_state = 0;
    uint64_t m_state_us = 0;
    int      m_state_extra;

    uint64_t m_last_us = 0;
    uint64_t m_brightness_us = 0;

    void update_value(int digit);

    // platform specific
    void update_brightness();
    void disable_cathode(int number);
    void enable_cathode(int number);

private:
    static bool m_hw_fade;
    static int brightnessToPwm(uint8_t brightness);
    static uint8_t pwmToBrightness(int pwm);
    void do_scroll();
    void do_overlap();
};

