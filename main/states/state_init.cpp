#include "state_init.h"
#include "../clock_hardware.h"
#include "clock_states.h"
#include "esp_timer.h"

static uint32_t start_us;

void state_init_on_enter(void)
{
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

void state_init_on_event(uint8_t event, uint8_t arg)
{
}

void state_init_on_exit(void)
{
}

