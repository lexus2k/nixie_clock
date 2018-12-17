#include "state_show_ip.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"

#include "esp_timer.h"
#include <sys/time.h>
#include <time.h>
#include <esp_wifi.h>
#include <lwip/ip_addr.h>

void StateShowIp::enter()
{
    display.set_mode( NixieDisplay::Mode::WRAP );
    tcpip_adapter_ip_info_t info;
    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA , &info) == ESP_OK)
    {
        char *ip = ip4addr_ntoa(&info.ip);
        display.set(ip);
    }
    m_start_us = (uint64_t)esp_timer_get_time();
}

void StateShowIp::run()
{
    uint32_t us = (uint64_t)esp_timer_get_time();
    if ( us - m_start_us > 10000000 )
    {
         pop_state();
    }
}

void StateShowIp::exit()
{
    display.set_mode( NixieDisplay::Mode::NORMAL );
}

uint8_t StateShowIp::get_id()
{
    return CLOCK_STATE_SHOW_IP;
}
