#include "state_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_events.h"
#include "clock_states.h"
#include "clock_time.h"
#include "http_server_task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "mdns.h"


void StateInit::enter()
{
    char s[CLOCK_TIME_FORMAT_SIZE];
    display.set(get_time_str(s,sizeof(s),nullptr));
    apply_settings();
    display.on();
    m_start_us = (uint64_t)esp_timer_get_time();
}

void StateInit::run()
{
    uint32_t us = (uint64_t)esp_timer_get_time();
    if ( us - m_start_us > 2000000 )
    {
         switch_state( CLOCK_STATE_MAIN );
    }
}

uint8_t StateInit::get_id()
{
    return CLOCK_STATE_APP_INIT;
}
