#pragma once

#include "sm_engine.h"

enum
{
    CLOCK_STATE_INIT,
    CLOCK_STATE_MAIN,
};

extern state_info_t clock_states[];
extern SmEngine states;

