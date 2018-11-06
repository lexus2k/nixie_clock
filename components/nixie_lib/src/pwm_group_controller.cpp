#include "pwm_group_controller.h"

#define TUBE_PWM_FREQ_HZ 200

#define MAX_PWM_VALUE  (1023)
#define MIN_PWM_VALUE  (MAX_PWM_VALUE * 72 / (1000000 / TUBE_PWM_FREQ_HZ))

#define BRIGHTNESS_UPDATE_PERIOD_US   20000
#define STEP_CYCLES_NUM (TUBE_PWM_FREQ_HZ / (1000000 / BRIGHTNESS_UPDATE_PERIOD_US))


PinGroupControllerPwm::PinGroupControllerPwm(gpio_num_t *pins,
                                             ledc_channel_t *channels,
                                             int count,
                                             uint32_t frequency)
    : m_channels(channels)
    , m_pins(pins)
    , m_count(count)
    , m_frequency(frequency)
    , m_min_pwm(MIN_PWM_VALUE)
    , m_max_pwm(MAX_PWM_VALUE)
{
}

void PinGroupControllerPwm::begin()
{
    init_ledc_timer(LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE);
    enable_hw_fade();
    for (int i=0; i<m_count; i++)
    {
        enable_pwm(m_pins[i], m_channels[i], LEDC_TIMER_0);
    }
}

void PinGroupControllerPwm::end()
{
    // TODO: Put disable pwm code
}

void PinGroupControllerPwm::update()
{
}

void PinGroupControllerPwm::set(int pin)
{
    set_pwm_hw(pin, 255);
}

void PinGroupControllerPwm::set(int n, uint8_t pwm)
{
    set_pwm_hw(n, pwm);
}

void PinGroupControllerPwm::clear(int pin)
{
    set_pwm_hw(pin, 0);
}

uint16_t PinGroupControllerPwm::byte_to_pwm(uint8_t data)
{
    uint16_t val = m_min_pwm + (uint32_t)data * (MAX_PWM_VALUE - MIN_PWM_VALUE) / 255;
    return val;
}

uint8_t PinGroupControllerPwm::pwm_to_byte(uint16_t pwm)
{
    if ( pwm < m_min_pwm ) pwm = m_min_pwm;
    if ( pwm > m_max_pwm ) pwm = m_max_pwm;
    uint8_t val = 255 * ((uint32_t)pwm - m_min_pwm) / ((m_max_pwm - m_min_pwm));
    return val;
}

void PinGroupControllerPwm::set_pwm_range(uint16_t min_pwm, uint16_t max_pwm)
{
    if ( min_pwm <= max_pwm )
    {
        m_min_pwm = min_pwm > MIN_PWM_VALUE ? min_pwm : MIN_PWM_VALUE;
        m_max_pwm = max_pwm < MAX_PWM_VALUE ? max_pwm : MAX_PWM_VALUE;
    }
}

void PinGroupControllerPwm::init_ledc_timer(ledc_timer_t timer, ledc_mode_t mode)
{
    ledc_timer_config_t ledc_timer{};
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz = m_frequency;
    ledc_timer.speed_mode = mode;
    ledc_timer.timer_num = timer;

    ledc_timer_config(&ledc_timer);
}

void PinGroupControllerPwm::enable_hw_fade()
{
    ledc_fade_func_install(0);
    m_hw_fade = true;
}

void PinGroupControllerPwm::disable_hw_fade()
{
    ledc_fade_func_uninstall();
    m_hw_fade = false;
}

void PinGroupControllerPwm::enable_pwm(gpio_num_t gpio, ledc_channel_t channel, ledc_timer_t timer)
{
    ledc_channel_config_t ledc_channel{};
    ledc_channel.channel    = channel;
    ledc_channel.duty       = 0;
    ledc_channel.gpio_num   = gpio;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel  = timer;
    ledc_channel_config(&ledc_channel);
}

void PinGroupControllerPwm::set_pwm_hw( int n, uint8_t data )
{
    if (m_hw_fade)
    {
        ledc_set_fade_with_step(LEDC_HIGH_SPEED_MODE, m_channels[n],
                                byte_to_pwm( data ), 1, STEP_CYCLES_NUM);
        ledc_fade_start(LEDC_HIGH_SPEED_MODE, m_channels[n], LEDC_FADE_NO_WAIT);
    }
    else
    {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, m_channels[n], byte_to_pwm( data ));
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, m_channels[n]);
    }
}
