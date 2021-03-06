#include "state_time_setup.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"
#include <sys/time.h>
#include <time.h>

enum
{
    SETUP_FIRST,
    SETUP_HOUR = SETUP_FIRST,
    SETUP_MIN,
    SETUP_SEC,
    SETUP_DAY,
    SETUP_MONTH,
    SETUP_YEAR,
    SETUP_LAST,
};

void StateTimeSetup::enter(SEventData *event)
{
    m_state = SETUP_FIRST;
    get_current_time( &m_time_info );
    update_display_content();
}

void StateTimeSetup::update()
{
    timeoutEvent( 10 * 1000000, true );
}

STransitionData StateTimeSetup::onEvent(SEventData event)
{
    //                 event id              event arg         transition_func          to state
    TRANSITION_SWITCH( SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            CLOCK_STATE_MAIN )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_1,     move_to_next_position() )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_2,     decrease_value() )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_3,     increase_value() )
    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_1,     save_time(),             CLOCK_STATE_MAIN )
    TRANSITION_TBL_END
}

void StateTimeSetup::move_to_next_position()
{
    resetTimeout();
    if ( ++m_state == SETUP_LAST ) m_state = SETUP_FIRST;
    update_display_content();
}

void StateTimeSetup::save_time()
{
    set_current_time( &m_time_info );
}

void StateTimeSetup::increase_value()
{
    resetTimeout();
    switch (m_state)
    {
        case SETUP_HOUR: if ( m_time_info.tm_hour++ == 23 ) m_time_info.tm_hour = 0; break;
        case SETUP_MIN: if ( m_time_info.tm_min++ == 59 ) m_time_info.tm_min = 0; break;
        case SETUP_SEC: if ( m_time_info.tm_sec++ == 59 ) m_time_info.tm_sec = 0; break;
        case SETUP_DAY: if ( m_time_info.tm_mday++ == 31 ) m_time_info.tm_mday = 1; break;
        case SETUP_MONTH: if ( m_time_info.tm_mon++ == 11 ) m_time_info.tm_mon = 0; break;
        case SETUP_YEAR: m_time_info.tm_year++; break;
        default: break;
    }
    update_display_content();
}

void StateTimeSetup::decrease_value()
{
    resetTimeout();
    switch (m_state)
    {
        case SETUP_HOUR: if ( m_time_info.tm_hour-- == 0 ) m_time_info.tm_hour = 23; break;
        case SETUP_MIN: if ( m_time_info.tm_min-- == 0 ) m_time_info.tm_min = 59; break;
        case SETUP_SEC: if ( m_time_info.tm_sec-- == 0 ) m_time_info.tm_sec = 59; break;
        case SETUP_DAY: if ( --m_time_info.tm_mday == 0 ) m_time_info.tm_mday = 31; break;
        case SETUP_MONTH: if ( m_time_info.tm_mon-- == 0 ) m_time_info.tm_mon = 11; break;
        case SETUP_YEAR: if ( --m_time_info.tm_year == 2019 ) m_time_info.tm_year = 2019; break;
        default: break;
    }
    update_display_content();
}

void StateTimeSetup::update_display_content()
{
    char s[CLOCK_TIME_FORMAT_SIZE];
    if ( m_state <= SETUP_SEC )
    {
        get_time_str(s, sizeof(s), &m_time_info);
        display.set( s );
        for (int i=0; i<6; i++)
        {
            display[i].set_effect( (i >> 1) == m_state ? NixieTubeAnimated::Effect::BLINK
                                                       : NixieTubeAnimated::Effect::IMMEDIATE );
        }
    }
    else
    {
        get_date_str(s, sizeof(s), &m_time_info);
        display.set( s );
        for (int i=0; i<6; i++)
        {
            display[i].set_effect( (i >> 1) == (m_state - SETUP_DAY) ? NixieTubeAnimated::Effect::BLINK
                                                                     : NixieTubeAnimated::Effect::IMMEDIATE );
        }
    }
}
