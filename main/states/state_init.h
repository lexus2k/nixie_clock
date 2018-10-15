#pragma once

#include "sm_engine.h"

#ifdef __cplusplus
extern "C"
{
#endif

void state_init_on_enter(void);
void state_init_main(void);
void state_init_on_event(uint8_t event, uint8_t arg);
void state_init_on_exit(void);

#ifdef __cplusplus
}
#endif
