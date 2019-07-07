#pragma once

#include "sm_state.h"
#include <time.h>

class StateSleep: public SmState
{
public:
    StateSleep(): SmState("sleep") {}

    void enter() override;
    void run() override;
    void exit() override;
    bool on_event(SEventData event) override;
    uint8_t get_id() override;
private:
};

