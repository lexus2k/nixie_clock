#include "state_show_ip.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_events.h"
#include "bluetooth/gatts_table.h"

#include <esp_wifi.h>
#include <lwip/ip_addr.h>

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

void StateShowIp::enter(SEventData *event)
{
    display.set_mode( NixieDisplay::Mode::WRAP, NixieDisplay::Mode::WRAP );
    tcpip_adapter_ip_info_t info;
    if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA , &info) == ESP_OK)
    {
        display.set(get_ip_string(&info.ip));
    }
//    clock_start_ble_service();
}

void StateShowIp::update()
{
    timeoutEvent( 60 * 1000000, true );
}

STransitionData StateShowIp::onEvent(SEventData event)
{
    //              event id              event arg         transition_func
    TRANSITION_POP( SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_POP( EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, sme::NO_FUNC() )
    TRANSITION_TBL_END
}

void StateShowIp::exit(SEventData *event)
{
//    clock_stop_ble_service();
    display.set_mode( NixieDisplay::Mode::NORMAL );
}


