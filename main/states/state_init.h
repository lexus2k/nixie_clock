#pragma once

#include "sm_engine.h"

class StateInit: public SmState
{
public:
    StateInit(): SmState("init") {}

    void enter() override;

    void update() override;

    EEventResult on_event(SEventData event) override;
};

