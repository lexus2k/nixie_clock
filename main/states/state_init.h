#pragma once

#include "sme/engine.h"

class StateInit: public SmState
{
public:
    StateInit(): SmState("init") {}

    void enter(SEventData *event) override;

    void update() override;

    void exit(SEventData *event) override;

    STransitionData onEvent(SEventData event) override;
};

