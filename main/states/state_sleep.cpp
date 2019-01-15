#include "state_sleep.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "nixie_clock.h"

#include <sys/time.h>
#include <time.h>

void StateSleep::enter()
{
    display.off(700000);
    leds.disable();
}

void StateSleep::run()
{
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

bool StateSleep::on_event(SEventData event)
{
    if ( event.event == EVT_BUTTON_PRESS && event.arg == BUTTON_1 )
    {
        pop_state();
        return true;
    }
    return false;
}

uint8_t StateSleep::get_id()
{
    return CLOCK_STATE_SLEEP;
}
