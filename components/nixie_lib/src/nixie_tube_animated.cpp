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
    uint64_t us = micros();
    switch (m_state)
    {
        case TUBE_OFF: break;
        case TUBE_NORMAL: break;
        case TUBE_SCROLL: do_scroll(); break;
        case TUBE_OVERLAP: do_overlap(); break;
        default: break;
    }
    NixieTubeBase::update();
    m_last_us = us;
}

void NixieTubeAnimated::set(int digit)
{
    update_value( digit );
    m_target_value = digit;
}

void NixieTubeAnimated::scroll(int value)
{
    m_state_us = micros();
    m_state = TUBE_SCROLL;
    m_state_extra = 0;
    m_target_value = value;
}

void NixieTubeAnimated::do_scroll()
{
    uint64_t us = micros();
    while ( us - m_state_us >= SCROLL_UPDATE_PERIOD_US )
    {
        int next = get_value() >= 9 ? 0 : (get_value() + 1);
        update_value( next );
        m_state_us += SCROLL_UPDATE_PERIOD_US;
        if ( get_value() == m_target_value )
        {
            if ( m_state_extra > 0 )
            {
                m_state = TUBE_NORMAL;
                break;
            }
            m_state_extra++;
        }
    }
}

void NixieTubeAnimated::do_overlap()
{
    uint64_t us = micros();
    while ( us - m_state_us >= SCROLL_UPDATE_PERIOD_US*2 )
    {
        update_value( m_target_value );
        m_state = TUBE_NORMAL;
        break;
    }
}

void NixieTubeAnimated::overlap(int value)
{
    m_state_us = micros();
    m_state = TUBE_OVERLAP;
    m_target_value = value;
    enable_cathode( value );
}
