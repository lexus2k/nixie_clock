#include "clock_audio.h"
#include "clock_hardware.h"
#include "audio_player.h"
#include "nixie_melodies.h"

#define DECLARE_BIN_FILE(x)  extern const uint8_t x##_start[] asm("_binary_"#x"_start"); \
                             extern const uint8_t x##_end[] asm("_binary_"#x"_end") \

#define DECLARE_MELODY(file, trackname, user_data) { \
        .notes = file##_start, \
        .data_len = static_cast<uint32_t>(file##_end - file##_start), \
        .type = MELODY_TYPE_VGM, \
        .pause = 0, \
        .track = 0, \
        .duration = 180000, \
        .customData = user_data, \
        .name = trackname }


DECLARE_BIN_FILE(wicked_child_vgm);
DECLARE_BIN_FILE(vampire_killer_vgm);
DECLARE_BIN_FILE(cave_explorer_vgm);
DECLARE_BIN_FILE(ice_path_vgm);
DECLARE_BIN_FILE(running_about_vgm);
//DECLARE_BIN_FILE(crysis_force_vgm);
//DECLARE_BIN_FILE(castle_beginning_vgm);
DECLARE_BIN_FILE(crisis_force_nsf);
DECLARE_BIN_FILE(castlevania3_nsf);

// IMPORTANT: Use 44.1kHz sound since it produces no noise
AudioPlayer audio_player(44100);

static NixieMelody melodies[] =
{
    DECLARE_MELODY(wicked_child_vgm, "Wicked Child", 0),
    DECLARE_MELODY(vampire_killer_vgm, "Vampire Killer", 0),
    DECLARE_MELODY(cave_explorer_vgm, "Cave Explorer", 0),
    DECLARE_MELODY(ice_path_vgm, "Ice Path", 0),
    DECLARE_MELODY(running_about_vgm, "Running About", 0),
//    DECLARE_MELODY(crysis_force_vgm, "Crisis Force", 200),
//    DECLARE_MELODY(castle_beginning_vgm, "Beginning", 100),
    DECLARE_MELODY(crisis_force_nsf, "Crisis Force", 200),
    DECLARE_MELODY(castlevania3_nsf, "Beginning", 100),
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
        audio_player.set_volume( 3.0f + static_cast<float>(melodies[index].customData) / 100.0f );
    }
    else
    {
        audio_player.set_volume( 0.3f );
    }
    audio_player.play( &melodies[index] );
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
    static const char *noname = "noname";
    if ( index >= 0 && index < audio_track_get_count() )
    {
        if ( melodies[index].name )
        {
            return melodies[index].name;
        }
    }
    return noname;
}
