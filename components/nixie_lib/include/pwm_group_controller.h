#pragma once

#include "pin_group_controller.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class PinGroupControllerPwm: public PinGroupController
{
public:
    PinGroupControllerPwm(gpio_num_t *pins, ledc_channel_t *channels, int count, uint32_t frequency);
    ~PinGroupControllerPwm() {};

    void begin() override;

    void end() override;

    void update() override;

    void set(int pin) override;

    bool set(int n, uint8_t pwm) override;

    void clear(int pin) override;

    void set_pwm_range(uint16_t min_pwm, uint16_t max_pwm);

private:
    ledc_channel_t *m_channels;
    gpio_num_t *m_pins;
    int m_count;
    uint32_t m_frequency;
    uint64_t m_brightness_us = 0;

    bool m_hw_fade;
    uint16_t byte_to_pwm(uint8_t data);
    uint8_t pwm_to_byte(uint16_t pwm);
    uint16_t m_min_pwm;
    uint16_t m_max_pwm;

    void enable_pwm(gpio_num_t gpio, ledc_channel_t channel, ledc_timer_t timer);
    void enable_hw_fade();
    void disable_hw_fade();

    void init_ledc_timer(ledc_timer_t timer = LEDC_TIMER_0, ledc_mode_t mode = LEDC_HIGH_SPEED_MODE);
    void set_pwm_hw( int n, uint16_t data );
};

