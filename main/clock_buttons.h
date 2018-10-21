#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void on_button_down(uint8_t id, uint16_t timeDeltaMs);
void on_button_up(uint8_t id, uint16_t timeDeltaMs);
void on_button_hold(uint8_t id, uint16_t timeDeltaMs);



#ifdef __cplusplus
}
#endif
