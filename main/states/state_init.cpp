#include "state_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_time.h"
#include "http_server_task.h"

#include "esp_log.h"

void StateInit::enter()
{
    char s[CLOCK_TIME_FORMAT_SIZE];
    display.set(get_time_str(s,sizeof(s),nullptr));
    apply_settings();
    display.on();
}

void StateInit::update()
{
    timeout_event( 1000000, true );
}

EEventResult StateInit::on_event(SEventData event)
{
    //             from state     event id              event arg      transition_func          type        to state
    SM_TRANSITION( SM_STATE_ANY,  SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,         SM_SWITCH,  CLOCK_STATE_MAIN );
    return EEventResult::NOT_PROCESSED;
}
