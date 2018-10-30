#include "clock_states.h"
#include "state_init.h"
#include "state_main.h"

state_info_t clock_states[] =
{
    SM_ENGINE_STATE(CLOCK_STATE_INIT,state_init),
    SM_ENGINE_STATE(CLOCK_STATE_MAIN,state_main),
    SM_ENGINE_STATE_END
};