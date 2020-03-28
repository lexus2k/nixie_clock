#pragma once

#include "sm_engine.h"

class StateHwInit: public SmState
{
public:
    StateHwInit(): SmState("hwinit") {}

    void enter() override;

    EEventResult on_event(SEventData event) override;
};

