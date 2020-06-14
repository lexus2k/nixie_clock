#include "state_sleep.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"

void StateSleep::enter(SEventData *event)
{
    display.off(700000);
    leds.off();
}

void StateSleep::exit(SEventData *event)
{
    display.on();
    display.update();
    if (settings.get_highlight_enable())
    {
        leds.on();
    }
}

STransitionData StateSleep::onEvent(SEventData event)
{
    //              event id              event arg         transition_func
    TRANSITION_POP( EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_TBL_END
}

