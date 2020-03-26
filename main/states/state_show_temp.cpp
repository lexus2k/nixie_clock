#include "state_show_temp.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_events.h"
#include "sm_engine2.h"

void StateShowTemp::enter()
{
    char str[18];
    int temp = temperature.get_celsius_hundreds();
    snprintf(str, sizeof(str)," %d  %d. %d  %d ",
        (temp/1000),
        (temp/100)%10,
        (temp/10)%10,
        (temp/1)%10);
    display.set_effect( NixieTubeAnimated::Effect::SCROLL );
    display.set(str);
}

void StateShowTemp::run()
{
    timeout_event( 10 * 1000000, true );
}

EEventResult StateShowTemp::on_event(SEventData event)
{
    //             from state     event id              event arg         transition_func          type       to state
    SM_TRANSITION( SM_STATE_ANY,  SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    return EEventResult::NOT_PROCESSED;
}
