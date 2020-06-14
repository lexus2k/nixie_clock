#include "state_show_temp.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_events.h"

void StateShowTemp::enter(SEventData *event)
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

void StateShowTemp::update()
{
    timeoutEvent( 10 * 1000000, true );
}

STransitionData StateShowTemp::onEvent(SEventData event)
{
    //              event id              event arg         transition_func
    TRANSITION_POP( SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_POP( EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_TBL_END
}
