#include "clock_events.h"
#include "states/nixie_clock.h"

extern NixieClock nixie_clock;

void send_app_event(uint8_t event_id, uint8_t arg)
{
    nixie_clock.sendEvent( (SEventData){event_id, arg} );
}

void send_delayed_app_event(uint8_t event_id, uint8_t arg, uint16_t timeoutMs)
{
    nixie_clock.sendEvent( (SEventData){event_id, arg}, timeoutMs );
}
