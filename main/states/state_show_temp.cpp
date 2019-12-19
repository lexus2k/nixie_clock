#include "state_show_temp.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"

void StateShowTemp::enter()
{
    char str[18];
    int temp = temperature.get_celsius_hundreds();
    snprintf(str, sizeof(str)," %d  %d. %d  %d ",
        (temp/1000),
        (temp/100)%10,
        (temp/10)%10,
        (temp/1)%10);
    display.set_effect( NixieTubeAnimated::Effect::SCROLL );
    display.set(str);
}

void StateShowTemp::run()
{
    if ( timeout_event( 10000000 ) )
    {
         pop_state();
    }
}

uint8_t StateShowTemp::get_id()
{
    return CLOCK_STATE_SHOW_TEMP;
}
