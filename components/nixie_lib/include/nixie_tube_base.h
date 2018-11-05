#pragma once

#include "pin_group_controller.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieTubeBase
{
public:
    NixieTubeBase(int cathodes_offset, PinGroupController* cathodes = nullptr);
    ~NixieTubeBase() = default;

    void set_anod(gpio_num_t pin);

    void set_cathodes(PinGroupController* cathodes)
    {
        m_cathodes = cathodes;
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
    int m_cathodes_offset = 0; // points to first cathode pin in group controller
    PinGroupController* m_cathodes = nullptr;
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


