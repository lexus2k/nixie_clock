#pragma once

#include "sm_engine.h"

class StateShowIp: public SmState
{
public:
    StateShowIp(): SmState("show_ip") {}

    void enter() override;

    void update() override;

    void exit() override;

    EEventResult on_event(SEventData event) override;
};

