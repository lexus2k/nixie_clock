#pragma once

#include "sm_engine.h"
#include "sm_state.h"

class StateShowIp: public SmState
{
public:
    StateShowIp(): SmState("show_ip") {}

    void enter() override;
    void run() override;
    uint8_t get_id() override;
private:
    uint32_t m_start_us;
};

