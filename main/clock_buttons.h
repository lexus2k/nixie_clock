#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    EVT_BUTTON_1,
    EVT_BUTTON_2,
    EVT_BUTTON_3,
    EVT_BUTTON_4,
};

void on_abutton_down(uint8_t id, uint16_t timeDeltaMs);
void on_abutton_up(uint8_t id, uint16_t timeDeltaMs);
void on_abutton_hold(uint8_t id, uint16_t timeDeltaMs);
void on_dbutton_down(uint8_t id, uint16_t timeDeltaMs);
void on_dbutton_up(uint8_t id, uint16_t timeDeltaMs);
void on_dbutton_hold(uint8_t id, uint16_t timeDeltaMs);

#if 0
void buttons_start_audio(void);
#endif


#ifdef __cplusplus
}
#endif
