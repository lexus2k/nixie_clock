#pragma once

#include "pin_muxers.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieTubeBase
{
public:
    NixieTubeBase(int index, PinMux* mux = nullptr)
        : m_index(index)
        , m_pinmux(mux) {};
    ~NixieTubeBase() = default;

    void set_anod(gpio_num_t pin);

    void set_pin_muxer(PinMux* muxer)
    {
        m_pinmux = muxer;
    }

    virtual void begin();
    virtual void end();
    virtual void update();
    void update_value(int digit);

    void on();
    void off();
    void set_brightness(uint8_t brightness);

    void enable_pwm(ledc_channel_t channel, ledc_timer_t timer = LEDC_TIMER_0);
    static void init_ledc_timer(ledc_timer_t timer = LEDC_TIMER_0, ledc_mode_t mode = LEDC_HIGH_SPEED_MODE);
    static void enable_hw_fade();
    static void disable_hw_fade();
    static void set_pwm_range(uint16_t min_pwm, uint16_t max_pwm);

protected:
    // platform specific
    void update_brightness();
    void disable_cathode(int number);
    void enable_cathode(int number);

    int get_value() { return m_value; };
    
private:
    int m_index = 0;
    PinMux* m_pinmux = nullptr;
    int m_pin = -1;
    bool m_enabled = false;
    int m_value = 0;
    bool m_pwmMode = false;
    ledc_channel_t m_channel;
    uint8_t m_brightness = 0;
    uint8_t m_target_brightness = 0;
    uint64_t m_brightness_us = 0;

    static bool m_hw_fade;
    static uint16_t brightnessToPwm(uint8_t brightness);
    static uint8_t pwmToBrightness(uint16_t pwm);
    static uint16_t m_min_pwm;
    static uint16_t m_max_pwm;
};

class NixieTubeAnimated: public NixieTubeBase
{
public:
    using NixieTubeBase::NixieTubeBase;

    void begin();
    void update() override;
    void end();

    void set(int digit);
    void scroll(int value);
    void overlap(int value);

protected:
    uint8_t  m_state = 0;
    uint64_t m_state_us = 0;
    int      m_state_extra;

    uint64_t m_last_us = 0;

private:
    int m_target_value = 0;

    void do_scroll();
    void do_overlap();
};

class NixieTube: public NixieTubeAnimated
{
public:
    using NixieTubeAnimated::NixieTubeAnimated;
};

