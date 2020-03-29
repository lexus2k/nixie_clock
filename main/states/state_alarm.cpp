#include "state_alarm.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_audio.h"
#include "clock_time.h"

void StateAlarm::enter()
{
    display.set_mode( NixieDisplay::Mode::NORMAL );
    display.set_effect( NixieTubeAnimated::Effect::BLINK );

    struct tm time_info{};
    time_info.tm_sec = 00;
    time_info.tm_min = settings.get_alarm() & 0xFF;
    time_info.tm_hour = (settings.get_alarm() >> 8) & 0xFF;
    char s[CLOCK_TIME_FORMAT_SIZE];
    get_time_str(s, sizeof(s), &time_info);
    display.set( s );

    audio_track_play( (settings.get_alarm() >> 16) & 0xFF );
}

void StateAlarm::update()
{
    timeout_event( 15 * 1000000, true );
}

EEventResult StateAlarm::on_event(SEventData event)
{
    //             from state     event id              event arg         transition_func          type       to state
    SM_TRANSITION( SM_STATE_ANY,  SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    return EEventResult::NOT_PROCESSED;
}

void StateAlarm::exit()
{
    audio_track_stop();

    display.set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
}


