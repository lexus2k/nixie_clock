#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool audio_track_play(int index);
int  audio_track_get_count(void);
const char *audio_track_get_name(int index);

#ifdef __cplusplus
}
#endif
