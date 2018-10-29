#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void on_abutton_down(uint8_t id, uint16_t timeDeltaMs);
void on_abutton_up(uint8_t id, uint16_t timeDeltaMs);
void on_abutton_hold(uint8_t id, uint16_t timeDeltaMs);
void on_dbutton_down(uint8_t id, uint16_t timeDeltaMs);
void on_dbutton_up(uint8_t id, uint16_t timeDeltaMs);
void on_dbutton_hold(uint8_t id, uint16_t timeDeltaMs);

void buttons_start_audio(void);


#ifdef __cplusplus
}
#endif
