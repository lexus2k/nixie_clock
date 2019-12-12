#include "state_show_ip.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"

//#include "esp_timer.h"
#include "platform/system.h"
#include <sys/time.h>
#include <time.h>
#include <esp_wifi.h>
#include <lwip/ip_addr.h>
#include "bluetooth/gatts_table.h"

std::string get_ip_string(const ip4_addr_t *addr)
{
    char *ip = ip4addr_ntoa(addr);
    std::string ip_str{};
    while (*ip)
    {
        ip_str += " ";
        ip_str += *ip;
        ip++;
        if ( *ip == '.' )
        {
            ip_str += '.';
            ip++;
        }
        else
        {
            ip_str += ' ';
        }
    }
    return ip_str;
}

void StateShowIp::enter()
{
    display.set_mode( NixieDisplay::Mode::WRAP );
    tcpip_adapter_ip_info_t info;
    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA , &info) == ESP_OK)
    {
        display.set(get_ip_string(&info.ip));
    }
    m_start_us = micros();
    clock_start_ble_service();
}

void StateShowIp::run()
{
    uint32_t us = micros();
    if ( (uint32_t)(us - m_start_us) > 30000000 )
    {
         pop_state();
    }
}

void StateShowIp::exit()
{
    clock_stop_ble_service();
    display.set_mode( NixieDisplay::Mode::NORMAL );
}

uint8_t StateShowIp::get_id()
{
    return CLOCK_STATE_SHOW_IP;
}
