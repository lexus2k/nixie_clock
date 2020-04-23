#include "clock_audio.h"
#include "clock_hardware.h"
#include "audio_player.h"
#include "nixie_melodies.h"

#define DECLARE_BIN_FILE(x)  extern const uint8_t x##_start[] asm("_binary_"#x"_start"); \
                             extern const uint8_t x##_end[] asm("_binary_"#x"_end") \

#define DECLARE_MELODY(file, trackname) { \
        .notes = file##_start, \
        .data_len = file##_end - file##_start, \
        .type = MELODY_TYPE_VGM, \
        .pause = 0, \
        .name = trackname }


DECLARE_BIN_FILE(wicked_child_vgm);
DECLARE_BIN_FILE(vampire_killer_vgm);
DECLARE_BIN_FILE(cave_explorer_vgm);
DECLARE_BIN_FILE(ice_path_vgm);
DECLARE_BIN_FILE(running_about_vgm);
DECLARE_BIN_FILE(crysis_force_vgm);


//extern const uint8_t running_about_vgm_start[] asm("_binary_running_about_vgm_start");
//extern const uint8_t running_about_vgm_end[]   asm("_binary_running_about_vgm_end");

static const char *noname = "noname";

// IMPORTANT: Use 16kHz sound since it produces no issues
// with Mario and Monkey Island melodies (8kHz has issues)
AudioPlayer audio_player(44100);

static NixieMelody melodies[] =
{
    DECLARE_MELODY(wicked_child_vgm, "Wicked Child"),
    DECLARE_MELODY(vampire_killer_vgm, "Vampire Killer"),
    DECLARE_MELODY(cave_explorer_vgm, "Cave Explorer"),
    DECLARE_MELODY(ice_path_vgm, "Ice Path"),
    DECLARE_MELODY(running_about_vgm, "Running About"),
    DECLARE_MELODY(crysis_force_vgm, "Crysis Force"),
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
        audio_player.set_volume( 3.0f );
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
