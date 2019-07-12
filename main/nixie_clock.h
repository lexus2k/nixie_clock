#pragma once

#include "sm_engine2.h"

class NixieClock: public SmEngine2
{
public:
    NixieClock();

    EEventResult on_event(SEventData event) override;

    void on_update() override;

    bool on_begin() override;

    void on_end() override;
};

