#include "state_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_events.h"
#include "clock_states.h"
#include "clock_time.h"
#include "esp_timer.h"

static uint32_t start_us;

static int main_events_hook( uint8_t event_id, uint8_t arg )
{
    if ( event_id == EVT_WIFI_CONNECTED )
    {
        leds.set_color( settings.get_color() );
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
    if ( event_id == EVT_UPGRADE_STATUS )
    {
        switch ( arg )
        {
            case EVT_UPGRADE_STARTED:
                leds.set_color(0, 0, 48);
                break;
            case EVT_UPGRADE_SUCCESS:
                leds.set_color( 0x007F00 );
                display.off();
                display.update();
                break;
            case EVT_UPGRADE_FAILED:
                leds.set_color(48, 0, 0);
                break;
            default: break;
        }
    }
    if ( event_id == EVT_BUTTON_PRESS && arg == 3 )
    {
         states.switch_state( CLOCK_STATE_SHOW_IP );
    }
    return 0;
}

void state_init_on_enter(void)
{
    char s[16];
    states.set_event_hook( main_events_hook );
    display.set(get_time_str(s,sizeof(s)));
    display.set_brightness(255);
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

