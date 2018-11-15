#pragma once

#include "sm_engine.h"

#ifdef __cplusplus
extern "C"
{
#endif

void state_show_ip_on_enter(void);
void state_show_ip_main(void);
int state_show_ip_on_event(uint8_t event, uint8_t arg);
void state_show_ip_on_exit(void);

#ifdef __cplusplus
}
#endif
