#include "state_show_temp.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"

//#include "esp_timer.h"
#include "platform/system.h"
#include <sys/time.h>
#include <time.h>
#include <esp_wifi.h>
#include <lwip/ip_addr.h>

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
    m_start_us = micros();
}

void StateShowTemp::run()
{
    uint32_t us = micros();
    if ( static_cast<uint32_t>(us - m_start_us) > 10000000 )
    {
         pop_state();
    }
}

uint8_t StateShowTemp::get_id()
{
    return CLOCK_STATE_SHOW_TEMP;
}
