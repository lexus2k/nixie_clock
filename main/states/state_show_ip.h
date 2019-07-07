#pragma once

#include "sm_state.h"

class StateShowIp: public SmState
{
public:
    StateShowIp(): SmState("show_ip") {}

    void enter() override;
    void run() override;
    void exit() override;
    uint8_t get_id() override;
private:
    uint32_t m_start_us;
};

