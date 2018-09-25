#include "nixie_tube.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void NixieTube::begin()
{
    if (m_pin >= 0)
    {
        gpio_set_direction(static_cast<gpio_num_t>(m_pin), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::end()
{
    if (m_pin >= 0)
    {
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::set(int digit)
{
    if (m_enabled)
    {
        m_pinmux.clear(m_index, m_digit);
        m_digit = digit;
        m_pinmux.set(m_index, m_digit);
    }
}

void NixieTube::off()
{
    if ( m_pwmMode )
    {
        setPwm( 0 );
        m_enabled = true;
    }
    else
    {
        m_pinmux.clear(m_index, m_digit);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
        m_enabled = false;
    }
}

void NixieTube::on()
{
    if ( m_pwmMode )
    {
        setPwm(brightnessToPwm(m_brightness));
        m_enabled = true;
    }
    else
    {
        m_pinmux.set(m_index, m_digit);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 1);
        m_enabled = true;
    }
}

void NixieTube::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
    if ( m_pwmMode && m_enabled )
    {
        setPwm(brightnessToPwm(m_brightness));
    }
}

void NixieTube::update()
{
}

void NixieTube::initLedcTimer(ledc_timer_t timer, ledc_mode_t mode)
{
    ledc_timer_config_t ledc_timer{};
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz = 500;
    ledc_timer.speed_mode = mode;
    ledc_timer.timer_num = timer;

    ledc_timer_config(&ledc_timer);
}

void NixieTube::enablePwm(ledc_channel_t channel, ledc_timer_t timer)
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

void NixieTube::setPwm(int value)
{
    if ( m_pwmMode )
    {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, m_channel, value);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, m_channel);
    }
}

int NixieTube::brightnessToPwm(uint8_t brightness)
{
    return (uint32_t)brightness * 1023 / 255;
}

uint8_t NixieTube::pwmToBrightness(int pwm)
{
    return (uint32_t)pwm * 255 / 1023;
}
