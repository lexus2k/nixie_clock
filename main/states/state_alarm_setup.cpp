#include "state_alarm_setup.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"
#include "clock_audio.h"
#include <sys/time.h>
#include <time.h>

enum
{
    SETUP_FIRST,
    SETUP_HOUR = SETUP_FIRST,
    SETUP_MIN,
    SETUP_MELODY,
    SETUP_LAST,

    SETUP_DAY,
    SETUP_MONTH,
    SETUP_YEAR,
};

void StateAlarmSetup::enter(SEventData *event)
{
    m_state = SETUP_FIRST;
    m_time_info.tm_sec = (settings.get_alarm() >> 16) & 0xFF;
    m_time_info.tm_min = settings.get_alarm() & 0xFF;
    m_time_info.tm_hour = (settings.get_alarm() >> 8) & 0xFF;
    m_enabled = !!(settings.get_alarm() >> 24);
    display[4].off();
    update_display_content();
}

void StateAlarmSetup::update()
{
    timeoutEvent( 10 * 1000000, true );
}

void StateAlarmSetup::exit(SEventData *event)
{
    display[4].on();
    audio_track_stop();
}

STransitionData StateAlarmSetup::onEvent(SEventData event)
{
    //                 event id              event arg         transition_func          to state
    TRANSITION_SWITCH( SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, audio_track_stop(),      CLOCK_STATE_MAIN );
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_1,     move_to_next_position() )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_2,     decrease_value() )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_3,     increase_value() )

    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_1,     save_time(),             CLOCK_STATE_MAIN )
    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_2,     audio_track_stop(),      CLOCK_STATE_MAIN )
    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_3,     audio_track_stop(),      CLOCK_STATE_MAIN )
    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_4,     audio_track_stop(),      CLOCK_STATE_MAIN )
    TRANSITION_TBL_END
}

void StateAlarmSetup::move_to_next_position()
{
    resetTimeout();
    if ( ++m_state == SETUP_LAST ) m_state = SETUP_FIRST;
    // reset effect for smooth switching
    display.set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
    update_display_content();
    if ( m_state == SETUP_MELODY )
    {
        if ( m_enabled ) audio_track_play( m_time_info.tm_sec );
    }
    else if ( m_state == SETUP_HOUR )
    {
        audio_track_stop();
    }
}

void StateAlarmSetup::save_time()
{
    // TODO: Write Alarm time
    uint32_t alarm = (m_time_info.tm_min << 0)  |
                     (m_time_info.tm_hour << 8) |
                     (m_time_info.tm_sec << 16) |
                     (m_enabled ? 0x01000000 : 0x00000000);
    settings.set_alarm( alarm );
    settings.save();
    audio_track_stop();
}

void StateAlarmSetup::increase_value()
{
    resetTimeout();
    switch (m_state)
    {
        case SETUP_HOUR: if ( m_time_info.tm_hour++ == 23 ) m_time_info.tm_hour = 0; break;
        case SETUP_MIN: if ( m_time_info.tm_min++ == 59 ) m_time_info.tm_min = 0; break;
        case SETUP_MELODY:
        {
            if ( !m_enabled ) m_enabled = true;
            else if ( m_time_info.tm_sec++ == audio_track_get_count() - 1 )
            {
                m_time_info.tm_sec = audio_track_get_count() - 1;
                m_enabled = false;
            }
            if ( m_enabled ) audio_track_play( m_time_info.tm_sec ); else audio_track_stop();
            break;
        }
        case SETUP_DAY: if ( m_time_info.tm_mday++ == 31 ) m_time_info.tm_mday = 1; break;
        case SETUP_MONTH: if ( m_time_info.tm_mon++ == 11 ) m_time_info.tm_mon = 0; break;
        case SETUP_YEAR: m_time_info.tm_year++; break;
        default: break;
    }
    update_display_content();
}

void StateAlarmSetup::decrease_value()
{
    resetTimeout();
    switch (m_state)
    {
        case SETUP_HOUR: if ( m_time_info.tm_hour-- == 0 ) m_time_info.tm_hour = 23; break;
        case SETUP_MIN: if ( m_time_info.tm_min-- == 0 ) m_time_info.tm_min = 59; break;
        case SETUP_MELODY:
        {
            if ( !m_enabled ) m_enabled = true;
            else if ( m_time_info.tm_sec-- == 0 )
            {
                m_time_info.tm_sec = 0;
                m_enabled = false;
            }
            if ( m_enabled ) audio_track_play( m_time_info.tm_sec ); else audio_track_stop();
            break;
        }
        case SETUP_DAY: if ( --m_time_info.tm_mday == 0 ) m_time_info.tm_mday = 31; break;
        case SETUP_MONTH: if ( m_time_info.tm_mon-- == 0 ) m_time_info.tm_mon = 11; break;
        case SETUP_YEAR: if ( --m_time_info.tm_year == 2019 ) m_time_info.tm_year = 2019; break;
        default: break;
    }
    update_display_content();
}

void StateAlarmSetup::update_display_content()
{
    char s[CLOCK_TIME_FORMAT_SIZE];
    if ( m_state <= SETUP_MELODY )
    {
        get_time_str(s, sizeof(s), &m_time_info);
        if ( m_time_info.tm_sec > 9 )
        {
            display[4].on();
        }
        else
        {
            display[4].off();
        }
        display.set( s );
        for (int i=0; i<6; i++)
        {
            bool blink = ((i >> 1) == m_state) || (m_state == SETUP_MELODY && !m_enabled);
            display[i].set_effect( blink ? NixieTubeAnimated::Effect::BLINK
                                         : NixieTubeAnimated::Effect::IMMEDIATE );
        }
    }
    else
    {
        // TODO: Music selection
        // TODO: Enable, disable Alarm
/*        get_date_str(s, sizeof(s), &m_time_info);
        display.set( s );
        for (int i=0; i<6; i++)
        {
            display[i].set_effect( (i >> 1) == (m_state - SETUP_DAY) ? NixieTubeAnimated::Effect::BLINK
                                                                     : NixieTubeAnimated::Effect::IMMEDIATE );
        }*/
    }
}
