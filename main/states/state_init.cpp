#include "state_init.h"
#include "clock_hardware.h"
#include "clock_events.h"
#include "clock_states.h"
#include "esp_timer.h"

static uint32_t start_us;

static int main_events_hook( uint8_t event_id, uint8_t arg )
{
    if ( event_id == EVT_WIFI_CONNECTED )
    {
        leds.set_color(2, 0, 64, 0);
        leds.set_color(3, 0, 64, 0);
    }
    if ( event_id == EVT_WIFI_FAILED )
    {
        leds.set_color(2, 64, 0, 0);
        leds.set_color(3, 64, 0, 0);
    }
    if ( event_id == EVT_WIFI_AP_MODE )
    {
        leds.set_color(2, 64, 64, 0);
        leds.set_color(3, 64, 64, 0);
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

