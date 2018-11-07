#include "nixie_tube_base.h"
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

#define BRIGHTNESS_UPDATE_PERIOD_US   20000

//bool NixieTubeBase::m_hw_fade = false;

NixieTubeBase::NixieTubeBase()
{
}

void NixieTubeBase::begin()
{
/*    if ( (m_pin >= 0) && (!m_pwmMode) )
    {
        gpio_set_direction(static_cast<gpio_num_t>(m_pin), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }*/
    if ( m_anod_offset >= 0 && m_anods )
    {
        m_anods->clear( m_anod_offset );
    }
    update_brightness();
}

void NixieTubeBase::end()
{
    if ( m_anod_offset >= 0 && m_anods )
    {
        m_anods->clear( m_anod_offset );
    }
/*    if (m_pin >= 0)
    {
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }*/
}

void NixieTubeBase::set_cathodes(int cathodes_offset, PinGroupController* cathodes)
{
    m_cathodes_offset = cathodes_offset;
    m_cathodes = cathodes;
}

void NixieTubeBase::set_anod(int anod_offset, PinGroupController* anods)
{
    m_anod_offset = anod_offset;
    m_anods = anods;
}

void NixieTubeBase::off()
{
    m_enabled = false;
    disable_cathodes();
    update_brightness();
}

void NixieTubeBase::on()
{
    m_enabled = true;
    enable_cathodes();
    update_brightness();
}

void NixieTubeBase::set_brightness(uint8_t brightness)
{
    m_target_brightness = brightness;
    m_brightness = brightness;
    update_brightness();
}

void NixieTubeBase::update()
{
//    uint64_t us = micros();
/*    if (!m_hw_fade)
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
    }*/
}

/*void NixieTubeBase::init_ledc_timer(ledc_timer_t timer, ledc_mode_t mode)
{
    ledc_timer_config_t ledc_timer{};
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;
    ledc_timer.freq_hz = TUBE_PWM_FREQ_HZ;
    ledc_timer.speed_mode = mode;
    ledc_timer.timer_num = timer;

    ledc_timer_config(&ledc_timer);
} */

/*void NixieTubeBase::enable_hw_fade()
{
    ledc_fade_func_install(0);
    m_hw_fade = true;
}

void NixieTubeBase::disable_hw_fade()
{
    ledc_fade_func_uninstall();
    m_hw_fade = false;
}*/

void NixieTubeBase::update_brightness()
{
    if ( !m_anods || m_anod_offset < 0 )
    {
        return;
    }
    if ( m_enabled )
    {
        m_anods->set( m_anod_offset, m_brightness );
    }
    else
    {
        m_anods->set( m_anod_offset, 0 );
    }
/*    if ( m_pwmMode )
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
    }*/
}

void NixieTubeBase::set(int digit)
{
    if ( digit < 10 )
    {
        set( '0' + digit );
    }
    else if (digit == 10)
    {
        set("~~.");
    }
    else if (digit == 11)
    {
        set(".~~");
    }
}

void NixieTubeBase::set(char ch)
{
    char str[2] = { ch, '\0' };
    set(str);
}

void NixieTubeBase::disable_cathode(int number)
{
    if ( m_cathodes != nullptr && (number >= 0) )
    {
        m_cathodes->clear(m_cathodes_offset + number);
    }
}

void NixieTubeBase::enable_cathode(int number)
{
    if (m_enabled && m_cathodes != nullptr && (number >= 0) )
    {
        m_cathodes->set(m_cathodes_offset + number);
    }
}

void NixieTubeBase::disable_cathodes()
{
    uint16_t cathodes = m_enabled_cathodes;
    int index = 0;
    while (cathodes)
    {
        if ( cathodes & 0x01 )
        {
            m_cathodes->clear(m_cathodes_offset + index);
        }
        index++;
        cathodes >>= 1;
    }
}

void NixieTubeBase::enable_cathodes()
{
    uint16_t cathodes = m_enabled_cathodes;
    int index = 0;
    while (cathodes)
    {
        if ( cathodes & 0x01 )
        {
            m_cathodes->set(m_cathodes_offset + index);
        }
        index++;
        cathodes >>= 1;
    }
}
