#pragma once

#include "sme/engine.h"
#include <time.h>

class StateSleep: public SmState
{
public:
    StateSleep(): SmState("sleep") {}

    void enter(SEventData *event) override;

    void exit(SEventData *event) override;

    STransitionData onEvent(SEventData event) override;
};

