#include "state_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_time.h"
#include "clock_audio.h"
#include "http_server_task.h"

#include "esp_log.h"

void StateInit::enter(SEventData *event)
{
    char s[CLOCK_TIME_FORMAT_SIZE];
    display.set(get_time_str(s,sizeof(s),nullptr));
    apply_settings();
    display.on();
}

void StateInit::update()
{
    timeoutEvent( 1000000, true );
}

STransitionData StateInit::onEvent(SEventData event)
{
    //                 event id              event arg      transition_func          to state
    TRANSITION_SWITCH( SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, sme::NO_FUNC(),         CLOCK_STATE_MAIN )
    TRANSITION_TBL_END
}

void StateInit::exit(SEventData *event)
{
    audio_sound_play( 0 );
}
