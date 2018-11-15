#include "state_show_ip.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"

#include "esp_timer.h"
#include <sys/time.h>
#include <time.h>
#include <esp_wifi.h>
#include <lwip/ip_addr.h>

static uint32_t start_us;

void state_show_ip_on_enter(void)
{
    tcpip_adapter_ip_info_t info;
    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA , &info) == ESP_OK)
    {
        char *ip = ip4addr_ntoa(&info.ip);
        display.set(&ip[strlen(ip) - 6]);
    }
    start_us = (uint64_t)esp_timer_get_time();
}

void state_show_ip_main(void)
{
    uint32_t us = (uint64_t)esp_timer_get_time();
    if ( us - start_us > 10000000 )
    {
         states.switch_state( CLOCK_STATE_MAIN );
    }
}

int state_show_ip_on_event(uint8_t event, uint8_t arg)
{
    return 0;
}

void state_show_ip_on_exit(void)
{
}

