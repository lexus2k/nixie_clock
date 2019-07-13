#include "clock_audio.h"
#include "clock_hardware.h"
#include "audio_player.h"
#include "nixie_melodies.h"

extern const uint8_t vkiller_vgm_start[] asm("_binary_vkiller_vgm_start");
extern const uint8_t vkiller_vgm_end[]   asm("_binary_vkiller_vgm_end");
static const char *noname = "noname";

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

bool audio_track_play(int index)
{
    if ( index < 0 || index >= audio_track_get_count() )
    {
        return false;
    }
    if ( melodies[index].type == MELODY_TYPE_VGM )
    {
        audio_player.set_volume( 3.5f );
        audio_player.play_vgm( &melodies[index] );
    }
    else
    {
        audio_player.set_volume( 0.3f );
        audio_player.play( &melodies[index] );
    }
    return true;
}

int  audio_track_get_count(void)
{
    return sizeof(melodies) / sizeof(NixieMelody);
}

const char *audio_track_get_name(int index)
{
    if ( index >= 0 && index < audio_track_get_count() )
    {
        if ( melodies[index].name )
        {
            return melodies[index].name;
        }
    }
    return noname;
}
