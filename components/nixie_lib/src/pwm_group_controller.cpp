#include "pwm_group_controller.h"

#define TUBE_PWM_FREQ_HZ 500

#define MAX_PWM_VALUE  (1023)
#define MIN_PWM_VALUE  (MAX_PWM_VALUE * 70 / (1000000 / TUBE_PWM_FREQ_HZ))
#define PWM_RANGE (MAX_PWM_VALUE - MIN_PWM_VALUE)

#define FULL_FADE_TIME_MS  (1000000)
#define FADE_STEP          (3)

#define STEP_CYCLES_NUM (TUBE_PWM_FREQ_HZ * FADE_STEP * (FULL_FADE_TIME_MS / PWM_RANGE) / 1000000 )


PinGroupControllerPwm::PinGroupControllerPwm()
    : m_min_pwm(MIN_PWM_VALUE)
    , m_max_pwm(MAX_PWM_VALUE)
{

}

PinGroupControllerPwm::PinGroupControllerPwm( const std::vector<pwn_pin_desc_t> &pins, uint32_t frequency)
    : m_min_pwm(MIN_PWM_VALUE)
    , m_max_pwm(MAX_PWM_VALUE)
{
    setup( pins, frequency );
}

void PinGroupControllerPwm::setup( const std::vector<pwn_pin_desc_t> &pins, uint32_t frequency )
{
    m_pins = pins;
    m_frequency = frequency;
}

void PinGroupControllerPwm::begin()
{
    init_ledc_timer(LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE);
    enable_hw_fade();
    for (int i=0; i<m_pins.size(); i++)
    {
        enable_pwm(m_pins[i].gpio, m_pins[i].channel, LEDC_TIMER_0);
    }
}

void PinGroupControllerPwm::end()
{
    for (int i=0; i<m_pins.size(); i++)
    {
        clear( i );
    }
}

void PinGroupControllerPwm::update()
{
}

void PinGroupControllerPwm::set(int pin)
{
    set_pwm_hw(pin, byte_to_pwm( 255 ));
}

bool PinGroupControllerPwm::set(int n, uint8_t pwm)
{
    set_pwm_hw(n, byte_to_pwm( pwm ));
    return true;
}

void PinGroupControllerPwm::clear(int pin)
{
    set_pwm_hw( pin, 0 );
}

uint16_t PinGroupControllerPwm::byte_to_pwm(uint8_t data)
{
    if (!data) return 0;
    uint16_t val = m_min_pwm + (uint32_t)data * (m_max_pwm - m_min_pwm) / 255;
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

void PinGroupControllerPwm::set_pwm_hw( int n, uint16_t data )
{
    if (m_hw_fade)
    {
        ledc_set_fade_step_and_start(LEDC_HIGH_SPEED_MODE, m_pins[n].channel,
                                data, FADE_STEP, STEP_CYCLES_NUM ? STEP_CYCLES_NUM : 1, LEDC_FADE_NO_WAIT);
    }
    else
    {
        ledc_set_duty_and_update( LEDC_HIGH_SPEED_MODE, m_pins[n].channel, data, 0xffff);
    }
}
