#pragma once

#include "sm_state.h"

class StateInit: public SmState
{
public:
    StateInit(): SmState("init") {}

    void enter() override;

    void run() override;

    EEventResult on_event(SEventData event) override;
};

