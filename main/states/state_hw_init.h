#pragma once

#include "sm_state.h"

class StateHwInit: public SmState
{
public:
    StateHwInit(): SmState("hwinit") {}

    void enter() override;
};

