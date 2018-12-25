#include "state_show_temp.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"

#include "esp_timer.h"
#include <sys/time.h>
#include <time.h>
#include <esp_wifi.h>
#include <lwip/ip_addr.h>

void StateShowTemp::enter()
{
    char str[18];
    int temp = temperature.get_celsius_hundreds();
    snprintf(str, sizeof(str),"%02d.%02d      ",temp/100,temp%100);
    display.set_effect( NixieDisplay::Effect::SCROLL );
    display.set(str);
    m_start_us = (uint64_t)esp_timer_get_time();
}

void StateShowTemp::run()
{
    uint32_t us = (uint64_t)esp_timer_get_time();
    if ( static_cast<uint32_t>(us - m_start_us) > 30000000 )
    {
         pop_state();
    }
}

uint8_t StateShowTemp::get_id()
{
    return CLOCK_STATE_SHOW_TEMP;
}
