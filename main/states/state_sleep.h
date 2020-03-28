#pragma once

#include "sm_engine.h"
#include <time.h>

class StateSleep: public SmState
{
public:
    StateSleep(): SmState("sleep") {}

    void enter() override;

    void exit() override;

    EEventResult on_event(SEventData event) override;
};

