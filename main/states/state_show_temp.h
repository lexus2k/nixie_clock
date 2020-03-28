#pragma once

#include "sm_engine.h"

class StateShowTemp: public SmState
{
public:
    StateShowTemp(): SmState("show_temp") {}

    void enter() override;

    void update() override;

    EEventResult on_event(SEventData event) override;
};

