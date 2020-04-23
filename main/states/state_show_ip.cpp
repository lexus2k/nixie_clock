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

void StateShowIp::enter()
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
    timeout_event( 60 * 1000000, true );
}

EEventResult StateShowIp::on_event(SEventData event)
{
    //             from state     event id              event arg         transition_func          type       to state
    SM_TRANSITION( SM_STATE_ANY,  SM_EVENT_TIMEOUT,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     SM_EVENT_ARG_ANY, SM_FUNC_NONE,            SM_POP,    SM_STATE_ANY );
    return EEventResult::NOT_PROCESSED;
}

void StateShowIp::exit()
{
//    clock_stop_ble_service();
    display.set_mode( NixieDisplay::Mode::NORMAL );
}


