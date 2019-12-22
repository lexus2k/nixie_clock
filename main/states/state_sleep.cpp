#include "state_sleep.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"
#include "sm_engine2.h"

void StateSleep::enter()
{
    display.off(700000);
    leds.disable();
}

void StateSleep::exit()
{
    display.on();
    display.update();
    if (settings.get_highlight_enable())
    {
        leds.enable();
    }
}

EEventResult StateSleep::on_event(SEventData event)
{
    SM_TRANSITION( SM_STATE_NONE, EVT_BUTTON_PRESS, SM_EVENT_ARG_NONE, SM_POP, SM_STATE_NONE );
    return EEventResult::NOT_PROCESSED;
}

