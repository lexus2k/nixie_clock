#include "clock_events.h"
#include "states/clock_states.h"

void send_app_event(uint8_t event_id, uint8_t arg)
{
    states.send_event( event_id, arg );
}
