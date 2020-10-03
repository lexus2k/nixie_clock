#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool audio_track_play(int index);
bool audio_sound_play(int index);
bool audio_track_is_playing(void);
void audio_track_stop(void);
void audio_set_volume(float vol);
float audio_get_volume(void);
int  audio_track_get_count(void);
const char *audio_track_get_name(int index);

#ifdef __cplusplus
}
#endif
