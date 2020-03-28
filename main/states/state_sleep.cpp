#include "state_sleep.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"

void StateSleep::enter()
{
    display.off(700000);
    leds.off();
}

void StateSleep::exit()
{
    display.on();
    display.update();
    if (settings.get_highlight_enable())
    {
        leds.on();
    }
}

EEventResult StateSleep::on_event(SEventData event)
{
    //             from state     event id              event arg         transition_func          type       to state
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    return EEventResult::NOT_PROCESSED;
}

