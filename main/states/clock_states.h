#pragma once

#include "sm_engine.h"

enum
{
    CLOCK_STATE_INIT,
    CLOCK_STATE_MAIN,
    CLOCK_STATE_SHOW_IP,
};

extern state_info_t clock_states[];
extern SmEngine states;

