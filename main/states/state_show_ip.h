#pragma once

#include "sm_state.h"

class StateShowIp: public SmState
{
public:
    StateShowIp(): SmState("show_ip") {}

    void enter() override;
    void run() override;
    void exit() override;

    EEventResult on_event(SEventData event) override;
};

