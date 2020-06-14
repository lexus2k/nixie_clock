#pragma once

#include "sme/engine.h"

class StateShowIp: public SmState
{
public:
    StateShowIp(): SmState("show_ip") {}

    void enter(SEventData *event) override;

    void update() override;

    void exit(SEventData *event) override;

    STransitionData onEvent(SEventData event) override;
};

