#include "state_init.h"
#include "../clock_hardware.h"
#include "../clock_events.h"
#include "clock_states.h"
#include "esp_timer.h"

static uint32_t start_us;

static int main_events_hook( uint8_t event_id, uint8_t arg )
{
    if ( event_id == EVT_WIFI_CONNECTED )
    {
    }
    return 0;
}

void state_init_on_enter(void)
{
    states.set_event_hook( main_events_hook );
    display.set_brightness(40);
    display.on();
    start_us = (uint64_t)esp_timer_get_time();
}

void state_init_main(void)
{
    uint32_t us = (uint64_t)esp_timer_get_time();
    if ( us - start_us > 2000000 )
    {
         states.switch_state( CLOCK_STATE_MAIN );
    }
}

int state_init_on_event(uint8_t event, uint8_t arg)
{
    return 0;
}

void state_init_on_exit(void)
{
}

