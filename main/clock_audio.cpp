#include "clock_audio.h"
#include "clock_hardware.h"
#include "audio_player.h"
#include "nixie_melodies.h"

extern const uint8_t wicked_child_vgm_start[] asm("_binary_wicked_child_vgm_start");
extern const uint8_t wicked_child_vgm_end[]   asm("_binary_wicked_child_vgm_end");

extern const uint8_t vampire_killer_vgm_start[] asm("_binary_vampire_killer_vgm_start");
extern const uint8_t vampire_killer_vgm_end[]   asm("_binary_vampire_killer_vgm_end");

extern const uint8_t cave_explorer_vgm_start[] asm("_binary_cave_explorer_vgm_start");
extern const uint8_t cave_explorer_vgm_end[]   asm("_binary_cave_explorer_vgm_end");

extern const uint8_t ice_path_vgm_start[] asm("_binary_ice_path_vgm_start");
extern const uint8_t ice_path_vgm_end[]   asm("_binary_ice_path_vgm_end");

//extern const uint8_t running_about_vgm_start[] asm("_binary_running_about_vgm_start");
//extern const uint8_t running_about_vgm_end[]   asm("_binary_running_about_vgm_end");

static const char *noname = "noname";

// IMPORTANT: Use 16kHz sound since it produces no issues
// with Mario and Monkey Island melodies (8kHz has issues)
AudioPlayer audio_player(16000);

static NixieMelody melodies[] =
{
    {
        .notes = wicked_child_vgm_start,
        .data_len = wicked_child_vgm_end - wicked_child_vgm_start,
        .type = MELODY_TYPE_VGM,
        .pause = 0,
        .name = "Wicked Child"
    },
    {
        .notes = vampire_killer_vgm_start,
        .data_len = vampire_killer_vgm_end - vampire_killer_vgm_start,
        .type = MELODY_TYPE_VGM,
        .pause = 0,
        .name = "Vampire Killer"
    },
    {
        .notes = cave_explorer_vgm_start,
        .data_len = cave_explorer_vgm_end - cave_explorer_vgm_start,
        .type = MELODY_TYPE_VGM,
        .pause = 0,
        .name = "Cave Explorer"
    },
    {
        .notes = ice_path_vgm_start,
        .data_len = ice_path_vgm_end - ice_path_vgm_start,
        .type = MELODY_TYPE_VGM,
        .pause = 0,
        .name = "Ice Path"
    },
/*    {
        .notes = running_about_vgm_start,
        .data_len = running_about_vgm_end - running_about_vgm_start,
        .type = MELODY_TYPE_VGM,
        .pause = 0,
        .name = "Running About"
    },*/
//    melodyMonkeyIslandP,
//    melodyMario2,
};

bool audio_track_is_playing(void)
{
    return audio_player.is_playing();
}

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

void audio_track_stop(void)
{
    audio_player.stop();
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
