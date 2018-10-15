#include "clock_states.h"
#include "state_init.h"

state_info_t clock_states[] =
{
    NIXIEOS_TASK(CLOCK_STATE_INIT,state_init),
    NIXIEOS_TASK_END
};

