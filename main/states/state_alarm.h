#pragma once

#include "sme/engine.h"

class StateAlarm: public SmState
{
public:
    StateAlarm(): SmState("alarm") {}

    void enter(SEventData *event) override;

    void update() override;

    void exit(SEventData *event) override;

    STransitionData onEvent(SEventData event) override;
};

