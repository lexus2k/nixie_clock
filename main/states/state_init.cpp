#include "state_init.h"
#include "../clock_hardware.h"

void state_init_on_enter(void)
{
    display.set_brightness(32);
    display.on();
}

void state_init_main(void)
{
   // TODO: swicth to main state after timeout
   // TODO: Add main state
}

void state_init_on_event(uint8_t event, uint8_t arg)
{
}

void state_init_on_exit(void)
{
}

