#pragma once

#include "pin_group_controller.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <vector>

typedef struct 
{
    gpio_num_t gpio;
    ledc_channel_t channel;
} pwn_pin_desc_t;

class PinGroupControllerPwm: public PinGroupController
{
public:
    PinGroupControllerPwm();
    PinGroupControllerPwm( const std::vector<pwn_pin_desc_t> &pins, uint32_t frequency);
    ~PinGroupControllerPwm() = default;

    void setup( const std::vector<pwn_pin_desc_t> &pins, uint32_t frequency );

    void begin() override;

    void end() override;

    void update() override;

    void set(int pin) override;

    bool set(int n, uint8_t pwm) override;

    void clear(int pin) override;

    void set_pwm_range(uint16_t min_pwm, uint16_t max_pwm);

    void set_pwm_range(int pin, uint16_t min_pwm, uint16_t max_pwm);

private:
    std::vector<pwn_pin_desc_t> m_pins;
    uint32_t m_frequency;
    uint64_t m_brightness_us = 0;

    bool m_hw_fade = false;
    uint16_t byte_to_pwm(int pin, uint8_t data);
    uint8_t pwm_to_byte(int pin, uint16_t pwm);
    std::vector<uint16_t> m_min_pwm;
    std::vector<uint16_t> m_max_pwm;

    void enable_pwm(gpio_num_t gpio, ledc_channel_t channel, ledc_timer_t timer);
    void enable_hw_fade();
    void disable_hw_fade();

    void init_ledc_timer(ledc_timer_t timer = LEDC_TIMER_0, ledc_mode_t mode = LEDC_HIGH_SPEED_MODE);
    void set_pwm_hw( int n, uint16_t data );
};

