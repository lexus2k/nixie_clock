#pragma once

#include "sme/engine.h"

class StateInit: public SmState
{
public:
    StateInit(): SmState("init") {}

    void enter(SEventData *event) override;

    void update() override;

    STransitionData onEvent(SEventData event) override;
};

