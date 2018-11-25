#pragma once

#include "sm_engine.h"

#ifdef __cplusplus
extern "C"
{
#endif

void state_main_on_enter(void);
void state_main_main(void);
int state_main_on_event(uint8_t event_id, uint8_t arg);
void state_main_on_exit(void);

#ifdef __cplusplus
}
#endif
