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

#define TUBE_PWM_FREQ_HZ   (140)
#define MAX_PWM_VALUE  (1023)
#define MIN_PWM_VALUE  (MAX_PWM_VALUE * 72 / (1000000 / TUBE_PWM_FREQ_HZ))

#define BRIGHTNESS_UPDATE_PERIOD_US   20000

#define STEP_CYCLES_NUM (TUBE_PWM_FREQ_HZ / (1000000 / BRIGHTNESS_UPDATE_PERIOD_US))

bool NixieTubeBase::m_hw_fade = false;
uint16_t NixieTubeBase::m_min_pwm = MIN_PWM_VALUE;
uint16_t NixieTubeBase::m_max_pwm = MAX_PWM_VALUE;

void NixieTubeBase::begin()
{
    if ( (m_pin >= 0) && (!m_pwmMode) )
    {
        gpio_set_direction(static_cast<gpio_num_t>(m_pin), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
    update_brightness();
}

void NixieTubeBase::end()
{
    if (m_pin >= 0)
    {
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTubeBase::set_anod(gpio_num_t pin)
{
    m_pin = static_cast<int>(pin);
}

void NixieTubeBase::off()
{
    m_pinmux->clear(m_index, m_value);
    m_enabled = false;
    update_brightness();
}

void NixieTubeBase::on()
{
    m_pinmux->set(m_index, m_value);
    m_enabled = true;
    update_brightness();
}

void NixieTubeBase::set_brightness(uint8_t brightness)
{
    m_target_brightness = brightness;
    update_brightness();
}

void NixieTubeBase::update()
{
    uint64_t us = micros();
    if (!m_hw_fade)
    {
        while ( us - m_brightness_us >= BRIGHTNESS_UPDATE_PERIOD_US )
        {
            m_brightness_us += BRIGHTNESS_UPDATE_PERIOD_US;
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
    }
}

void NixieTubeBase::init_ledc_timer(ledc_timer_t timer, ledc_mode_t mode)
{
    ledc_timer_config_t ledc_timer{};
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz = TUBE_PWM_FREQ_HZ;
    ledc_timer.speed_mode = mode;
    ledc_timer.timer_num = timer;

    ledc_timer_config(&ledc_timer);
}

void NixieTubeBase::enable_hw_fade()
{
    ledc_fade_func_install(0);
    m_hw_fade = true;
}

void NixieTubeBase::disable_hw_fade()
{
    ledc_fade_func_uninstall();
    m_hw_fade = false;
}

void NixieTubeBase::enable_pwm(ledc_channel_t channel, ledc_timer_t timer)
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

uint16_t NixieTubeBase::brightnessToPwm(uint8_t brightness)
{
    uint16_t val = m_min_pwm + (uint32_t)brightness * (MAX_PWM_VALUE - MIN_PWM_VALUE) / 255;
    return val;
}

uint8_t NixieTubeBase::pwmToBrightness(uint16_t pwm)
{
    if ( pwm < m_min_pwm ) pwm = m_min_pwm;
    if ( pwm > m_max_pwm ) pwm = m_max_pwm;
    uint8_t val = 255 * ((uint32_t)pwm - m_min_pwm) / ((m_max_pwm - m_min_pwm));
    return val;
}

void NixieTubeBase::set_pwm_range(uint16_t min_pwm, uint16_t max_pwm)
{
    if ( min_pwm <= max_pwm )
    {
        m_min_pwm = min_pwm > MIN_PWM_VALUE ? min_pwm : MIN_PWM_VALUE;
        m_max_pwm = max_pwm < MAX_PWM_VALUE ? max_pwm : MAX_PWM_VALUE;
    }
}

void NixieTubeBase::update_brightness()
{
    if ( m_pwmMode )
    {
        if ( m_enabled )
        {
            if (m_hw_fade)
            {
                ledc_set_fade_with_step(LEDC_HIGH_SPEED_MODE, m_channel,
                                        brightnessToPwm( m_brightness ), 1, STEP_CYCLES_NUM);
                ledc_fade_start(LEDC_HIGH_SPEED_MODE, m_channel, LEDC_FADE_NO_WAIT);
            }
            else
            {
                ledc_set_duty(LEDC_HIGH_SPEED_MODE, m_channel, brightnessToPwm( m_brightness ));
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, m_channel);
            }
        }
        else
        {
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, m_channel, 0);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, m_channel);
        }
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

void NixieTubeBase::update_value(int digit)
{
    disable_cathode( m_value );
    m_value = digit;
    enable_cathode( m_value );
}

void NixieTubeBase::disable_cathode(int number)
{
    if ( m_pinmux != nullptr )
    {
        m_pinmux->clear(m_index, number);
    }
}

void NixieTubeBase::enable_cathode(int number)
{
    if (m_enabled && m_pinmux != nullptr )
    {
        m_pinmux->set(m_index, number);
    }
}

