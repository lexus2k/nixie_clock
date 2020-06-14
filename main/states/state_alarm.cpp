#include "state_alarm.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_audio.h"
#include "clock_time.h"

void StateAlarm::enter(SEventData *event)
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
    timeoutEvent( 60 * 1000000, true );
    if ( !audio_track_is_playing() )
    {
        audio_track_play( (settings.get_alarm() >> 16) & 0xFF );
    }
}

STransitionData StateAlarm::onEvent(SEventData event)
{
    //              event id              event arg         transition_func
    TRANSITION_POP( SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_POP( EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_TBL_END
}

void StateAlarm::exit(SEventData *event)
{
    audio_track_stop();

    display.set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
}


