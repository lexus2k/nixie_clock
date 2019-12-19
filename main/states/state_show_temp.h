#pragma once

#include "sm_state.h"

class StateShowTemp: public SmState
{
public:
    StateShowTemp(): SmState("show_temp") {}

    void enter() override;
    void run() override;
    uint8_t get_id() override;
};

