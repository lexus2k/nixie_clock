#pragma once

#include "sme/engine.h"

class StateShowTemp: public SmState
{
public:
    StateShowTemp(): SmState("show_temp") {}

    void enter(SEventData *event) override;

    void update() override;

    STransitionData onEvent(SEventData event) override;
};

