#pragma once

#include "sm_engine2.h"
#include "state_main.h"
#include "state_hw_init.h"
#include "state_init.h"
#include "state_show_ip.h"
#include "state_sleep.h"

class NixieClock: public SmEngine2
{
public:
    NixieClock();

    bool on_event(SEventData event) override;
private:
	StateHwInit m_hw_init;
    StateInit m_init;
    StateMain m_main;
    StateShowIp m_show_ip;
    StateSleep m_sleep;
};

