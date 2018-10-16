#include "nixie_tube.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static uint64_t micros()
{
    return (uint64_t)esp_timer_get_time();
}

#define TUBE_PWM_FREQ_HZ   (500)
#define MAX_PWM_VALUE  (1023)
#define MIN_PWM_VALUE  (MAX_PWM_VALUE * 72 / (1000000 / TUBE_PWM_FREQ_HZ))

#define BRIGHTNESS_UPDATE_PERIOD_US   20000

enum
{
    TUBE_OFF = 0,
    TUBE_NORMAL,
    TUBE_SMOOTH_ON,
};

void NixieTube::begin()
{
    m_last_us = micros();
    if ( (m_pin >= 0) && (!m_pwmMode) )
    {
        gpio_set_direction(static_cast<gpio_num_t>(m_pin), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
    update_brightness();
}

void NixieTube::end()
{
    if (m_pin >= 0)
    {
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::set_anod(gpio_num_t pin)
{
    m_pin = static_cast<int>(pin);
}

void NixieTube::set(int digit)
{
    update_value( digit );
    m_target_value = digit;
}

void NixieTube::off()
{
    m_pinmux->clear(m_index, m_value);
    m_enabled = false;
    update_brightness();
}

void NixieTube::on()
{
    m_pinmux->set(m_index, m_value);
    m_enabled = true;
    m_state = TUBE_NORMAL;
    update_brightness();
}

void NixieTube::set_brightness(uint8_t brightness)
{
//    m_brightness = brightness;
    m_target_brightness = brightness;
    update_brightness();
}

void NixieTube::set_smooth_brightness(uint8_t brightness)
{
    m_target_brightness = brightness;
}

void NixieTube::update()
{
    uint64_t us = micros();
    switch (m_state)
    {
        case TUBE_OFF: break;
        case TUBE_NORMAL: break;
        case TUBE_SMOOTH_ON:
        {
            break;
        }
        default: break;
    }
    uint64_t time_us = m_last_us;
    while ( us - time_us >= BRIGHTNESS_UPDATE_PERIOD_US )
    {
        time_us += BRIGHTNESS_UPDATE_PERIOD_US;
        if ( m_brightness < m_target_brightness )
        {
//            fprintf(stderr, "[%llu][%d] -> [%d]\n", time_us, m_brightness, m_target_brightness);
            m_brightness++;
        }
        else if ( m_brightness > m_target_brightness )
        {
            m_brightness--;
        }
        else
        {
            break;
        }
        update_brightness();
    }
    m_last_us = us;
}

void NixieTube::initLedcTimer(ledc_timer_t timer, ledc_mode_t mode)
{
    ledc_timer_config_t ledc_timer{};
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz = TUBE_PWM_FREQ_HZ;
    ledc_timer.speed_mode = mode;
    ledc_timer.timer_num = timer;

    ledc_timer_config(&ledc_timer);
}

void NixieTube::enable_pwm(ledc_channel_t channel, ledc_timer_t timer)
{
    if ( m_pin < 0 )
    {
        printf("[WARN] Anoid pin is not set for nixie tube\n");
        return;
    }
    m_channel = channel;

    ledc_channel_config_t ledc_channel{};
    ledc_channel.channel    = m_channel;
    ledc_channel.duty       = 0;
    ledc_channel.gpio_num   = static_cast<gpio_num_t>(m_pin);
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel  = timer;
    ledc_channel_config(&ledc_channel);
    m_pwmMode = true;
}

int NixieTube::brightnessToPwm(uint8_t brightness)
{
    int val = MIN_PWM_VALUE + (uint32_t)brightness * (MAX_PWM_VALUE - MIN_PWM_VALUE) / 255;
    return val;
}

uint8_t NixieTube::pwmToBrightness(int pwm)
{
    if ( pwm < MIN_PWM_VALUE )
    {
        pwm = MIN_PWM_VALUE;
    }
    uint8_t val = 255 * ((uint32_t)pwm - MIN_PWM_VALUE) / ((MAX_PWM_VALUE - MIN_PWM_VALUE));
    return val;
}

void NixieTube::update_brightness()
{
    if ( m_pwmMode )
    {
        if ( m_enabled )
        {
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, m_channel, brightnessToPwm( m_brightness ));
        }
        else
        {
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, m_channel, 0);
        }
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, m_channel);
    }
    else if (m_pin >= 0)
    {
        if ( m_enabled && m_brightness > 0 )
        {
            gpio_set_level(static_cast<gpio_num_t>(m_pin), 1);
        }
        else
        {
            gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
        }
    }
}

void NixieTube::update_value(int digit)
{
    if ( m_pinmux != nullptr )
    {
        m_pinmux->clear(m_index, m_value);
    }
    m_value = digit;
    if (m_enabled && m_pinmux != nullptr )
    {
        m_pinmux->set(m_index, m_value);
    }
}
