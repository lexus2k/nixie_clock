#include "clock_audio.h"
#include "clock_hardware.h"
#include "audio_player.h"
#include "nixie_melodies.h"

extern const uint8_t vkiller_vgm_start[] asm("_binary_vkiller_vgm_start");
extern const uint8_t vkiller_vgm_end[]   asm("_binary_vkiller_vgm_end");

AudioPlayer audio_player(8000);

static NixieMelody melodies[] =
{
    {
        .notes = vkiller_vgm_start,
        .data_len = vkiller_vgm_end - vkiller_vgm_start,
        .type = MELODY_TYPE_VGM,
        .pause = 0,
        .name = "Vampire Killer"
    },
    melodyMonkeyIslandP,
    melodyMario2,
};
