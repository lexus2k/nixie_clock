#include "clock_events.h"
#include "states/state_engine.h"

extern NixieClock nixie_clock;

void send_app_event(uint8_t event_id, uint8_t arg)
{
    nixie_clock.send_event( (SEventData){event_id, arg} );
}
