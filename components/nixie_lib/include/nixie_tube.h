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

    void begin();
    void end();
    void update();

    void enablePwm(ledc_channel_t channel, ledc_timer_t timer = LEDC_TIMER_0);
    static void initLedcTimer(ledc_timer_t timer = LEDC_TIMER_0, ledc_mode_t mode = LEDC_HIGH_SPEED_MODE);

protected:
    int m_index = 0;
    PinMux* m_pinmux = nullptr;
    int m_pin = -1;
    bool m_enabled = false;
    int m_digit = 0;
    bool m_pwmMode = false;
    ledc_channel_t m_channel;
    uint8_t m_brightness = 255;

    void setPwm(int value);

private:
    static int brightnessToPwm(uint8_t brightness);
    static uint8_t pwmToBrightness(int pwm);
};

