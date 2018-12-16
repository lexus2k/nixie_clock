#include "nixie_tube_animated.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#define SCROLL_UPDATE_PERIOD_US  30000

static uint64_t micros()
{
    return (uint64_t)esp_timer_get_time();
}

enum
{
    TUBE_OFF = 0,
    TUBE_NORMAL,
    TUBE_SCROLL,
    TUBE_OVERLAP,
};

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
    switch (m_state.index)
    {
        case TUBE_OFF: break;
        case TUBE_NORMAL: break;
        case TUBE_SCROLL: do_scroll(); break;
        case TUBE_OVERLAP: do_overlap(); break;
        default: break;
    }
    NixieTubeBase::update();
    if ( m_off_us != 0 && m_off_us < m_last_us )
    {
        m_off_us = 0;
        off();
    }
}

void NixieTubeAnimated::set(int digit)
{
    disable_cathode( m_state.value );
    enable_cathode( digit );
    m_state.value = digit;
}

void NixieTubeAnimated::off(uint32_t delay_us)
{
    fake_off();
    m_off_us = m_last_us + delay_us;
}

void NixieTubeAnimated::set_effect(NixieTubeAnimated::Effect effect)
{
    m_state.effect = effect;
}

void NixieTubeAnimated::animate(int value)
{
    switch (m_state.effect)
    {
        case Effect::SCROLL:
            scroll( value );
            break;
        case Effect::OVERLAP:
            overlap( value );
            break;
        case Effect::IMMEDIATE:
        default:
            set( value );
            break;
    }
}

void NixieTubeAnimated::scroll(int value)
{
    m_state.timestamp_us = m_last_us;
    m_state.index = TUBE_SCROLL;
    m_state.extra = 0;
    m_state.target_value = value;
    if (m_state.value < 0) m_state.value = 9;
}

void NixieTubeAnimated::do_scroll()
{
    uint64_t us = m_last_us;
    while ( us - m_state.timestamp_us >= SCROLL_UPDATE_PERIOD_US )
    {
        int next = m_state.value >= 9 ? 0 : (m_state.value + 1);
        disable_cathode( m_state.value );
        enable_cathode( next );
        m_state.value = next;
        m_state.timestamp_us += SCROLL_UPDATE_PERIOD_US;
        if ( m_state.value == m_state.target_value ||
             ( m_state.value == 0 && m_state.target_value < 0 ) )
        {
            if ( m_state.extra > 0 )
            {
                if (m_state.target_value < 0)
                {
                    disable_cathode( next );
                }
                m_state.index = TUBE_NORMAL;
                break;
            }
            m_state.extra++;
        }
    }
}

void NixieTubeAnimated::do_overlap()
{
    uint64_t us = m_last_us;
    while ( us - m_state.timestamp_us >= SCROLL_UPDATE_PERIOD_US*2 )
    {
        disable_cathode( m_state.value );
        enable_cathode( m_state.target_value );
        m_state.value = m_state.target_value;
        m_state.index = TUBE_NORMAL;
        break;
    }
}

void NixieTubeAnimated::overlap(int value)
{
    m_state.timestamp_us = m_last_us;
    m_state.index = TUBE_OVERLAP;
    m_state.target_value = value;
    enable_cathode( value );
}
