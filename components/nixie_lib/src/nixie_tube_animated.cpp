#include "nixie_tube_animated.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SCROLL_UPDATE_PERIOD_US  20000

static uint64_t micros()
{
    return (uint64_t)esp_timer_get_time();
}

void NixieTubeAnimated::begin()
{
    m_last_us = micros();
    NixieTubeBase::begin();
}

void NixieTubeAnimated::end()
{
    NixieTubeBase::end();
}

void NixieTubeAnimated::update()
{
    m_last_us = micros();
    if ( m_state.effect_active )
    {
        switch (m_state.effect)
        {
            case Effect::IMMEDIATE: m_state.effect_active = false; break;
            case Effect::SCROLL: do_scroll(); break;
            case Effect::OVERLAP: do_overlap(); break;
            case Effect::BLINK: do_blink(); break;
            default: break;
        }
    }
    NixieTubeBase::update();
    if ( m_off_us != 0 && m_off_us < m_last_us )
    {
        m_off_us = 0;
        off();
    }
}

std::string NixieTubeAnimated::get_content()
{
    int digit = get_enabled_cathode();
    int ldot = get_enabled_cathode( 9 );
    int rdot = get_enabled_cathode( 10 );
    if ( digit > 9 )
    {
        digit = -1;
    }
    if ( ldot > 10 )
    {
        ldot = -1;
    }
    std::string result = "";
    result += ldot < 0 ? ' ': '.';
    result += digit < 0 ? ' ': ('0' + digit);
    result += rdot < 0 ? ' ': '.';
    return result;
}

void NixieTubeAnimated::off(uint32_t delay_us)
{
    fake_off();
    m_off_us = m_last_us + delay_us;
}

void NixieTubeAnimated::set_effect(NixieTubeAnimated::Effect effect)
{
    if ( m_state.effect == effect )
    {
        return;
    }
    reset_effect();
    m_state.extra = 0;
    m_state.effect = effect;
    m_state.timestamp_us = m_last_us;
    if ( effect == Effect::BLINK )
    {
        m_state.effect_active = true;
    }
}

void NixieTubeAnimated::reset_effect()
{
    if ( !m_state.effect_active )
    {
        return;
    }
    switch (m_state.effect)
    {
        case Effect::SCROLL:
            disable_cathode( m_state.value );
            enable_cathode( m_state.target_value );
            m_state.value = m_state.target_value;
            break;
        case Effect::OVERLAP:
            disable_cathode( m_state.value );
            enable_cathode( m_state.target_value );
            m_state.value = m_state.target_value;
            break;
        case Effect::BLINK:
            // Return normal brightness if switched to non-blink mode
            set_user_brightness_fraction(100);
            break;
        case Effect::IMMEDIATE:
        default:
            // Nothing to to do
            break;
    }
    m_state.effect_active = false;
}

void NixieTubeAnimated::animate(int value)
{
    switch (m_state.effect)
    {
        case Effect::SCROLL:
            reset_effect();
            m_state.timestamp_us = m_last_us;
            m_state.extra = 0;
            m_state.target_value = value;
            if ( m_state.value < 0 )
            {
                m_state.value = 9;
                enable_cathode( 9 );
            }
            break;
        case Effect::OVERLAP:
            reset_effect();
            m_state.timestamp_us = m_last_us;
            m_state.target_value = value;
            enable_cathode( m_state.target_value );
            break;
        case Effect::BLINK:
        case Effect::IMMEDIATE:
        default:
            disable_cathode( m_state.value );
            m_state.target_value = value;
            m_state.value = value;
            enable_cathode( m_state.value );
            // set( value );
            break;
    }
    m_state.effect_active = true;
}

void NixieTubeAnimated::do_scroll()
{
    while ( static_cast<uint64_t>(m_last_us - m_state.timestamp_us) >= SCROLL_UPDATE_PERIOD_US )
    {
        disable_cathode( m_state.value );
        m_state.value = m_state.value >= 9 ? 0 : (m_state.value + 1);
        enable_cathode( m_state.value );
        m_state.timestamp_us += SCROLL_UPDATE_PERIOD_US;
        if ( m_state.value == m_state.target_value ||
             ( m_state.value == 0 && m_state.target_value < 0 ) )
        {
            if ( m_state.extra > 0 )
            {
                if (m_state.target_value < 0)
                {
                    disable_cathode( m_state.value );
                }
                m_state.effect_active = false;
                break;
            }
            m_state.extra++;
        }
    }
}

void NixieTubeAnimated::do_overlap()
{
    uint64_t delta = static_cast<uint64_t>(m_last_us - m_state.timestamp_us);
    if ( delta >= 100000 )
    {
        disable_cathode( m_state.value );
        enable_cathode( m_state.target_value );
        m_state.value = m_state.target_value;
        m_state.effect_active = false;
    }
}

void NixieTubeAnimated::do_blink()
{
    uint64_t us = m_last_us;
    uint8_t fraction = get_user_brightness_fraction();
    while ( static_cast<uint64_t>(us - m_state.timestamp_us) >= 5000 )
    {
        if ( m_state.extra == 0 ) // going down
        {
            if ( fraction == 0 ) { fraction = 1; m_state.extra = 1; } else { fraction--; };
        }
        else // going up
        {
            if ( fraction == 100 ) { fraction = 99; m_state.extra = 0; } else { fraction++; };
        }
        m_state.timestamp_us += 5000;
    }
    set_user_brightness_fraction( fraction );
}

