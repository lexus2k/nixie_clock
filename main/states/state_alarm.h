#pragma once

#include "sm_engine.h"

class StateAlarm: public SmState
{
public:
    StateAlarm(): SmState("alarm") {}

    void enter() override;

    void update() override;

    void exit() override;

    EEventResult on_event(SEventData event) override;
};

