#pragma once

#include "sme/engine.h"

class StateHwInit: public SmState
{
public:
    StateHwInit(): SmState("hwinit") {}

    void enter(SEventData *event) override;

    STransitionData onEvent(SEventData event) override;
};

