#include "clock_audio.h"
#include "clock_hardware.h"
#include "audio_player.h"
#include "nixie_melodies.h"

#define DECLARE_BIN_FILE(x)  extern const uint8_t x##_start[] asm("_binary_"#x"_start"); \
                             extern const uint8_t x##_end[] asm("_binary_"#x"_end") \

#define DECLARE_VGM_MELODY(file, trackname, user_data) { \
        .notes = file##_start, \
        .data_len = static_cast<uint32_t>(file##_end - file##_start), \
        .type = MELODY_TYPE_VGM, \
        .pause = 0, \
        .track = 0, \
        .duration = 0, \
        .customData = user_data, \
        .name = trackname }

#define DECLARE_NSF_MELODY(file, trackname, trackid, dur, user_data) { \
        .notes = file##_start, \
        .data_len = static_cast<uint32_t>(file##_end - file##_start), \
        .type = MELODY_TYPE_NSF, \
        .pause = 0, \
        .track = trackid, \
        .duration = dur, \
        .customData = user_data, \
        .name = trackname }


DECLARE_BIN_FILE(wicked_child_vgm);
DECLARE_BIN_FILE(vampire_killer_vgm);
DECLARE_BIN_FILE(cave_explorer_vgm);
DECLARE_BIN_FILE(ice_path_vgm);
DECLARE_BIN_FILE(mario_nsf);
DECLARE_BIN_FILE(mario3_nsf);
DECLARE_BIN_FILE(crisis_force_nsf);
DECLARE_BIN_FILE(castlevania3_nsf);
DECLARE_BIN_FILE(contra_nsf);
DECLARE_BIN_FILE(tinytoon_nsf);


AudioPlayer audio_player(44100);

static const NixieMelody melodies[] =
{
    DECLARE_VGM_MELODY(wicked_child_vgm, "Wicked Child", 0),
    DECLARE_VGM_MELODY(vampire_killer_vgm, "Vampire Killer", 0),
    DECLARE_VGM_MELODY(cave_explorer_vgm, "Adventure Cave Explorer", 0),
    DECLARE_VGM_MELODY(ice_path_vgm, "Adventure Ice Path", 0),
    DECLARE_NSF_MELODY(mario_nsf, "Mario Running About", 0, 180000, 100),
    DECLARE_NSF_MELODY(mario3_nsf, "Mario3 Theme 8", 8, 180000, 100),
    DECLARE_NSF_MELODY(mario3_nsf, "Mario3 Theme 9", 9, 180000, 100),
    DECLARE_NSF_MELODY(crisis_force_nsf, "Crisis Force Stage 1", 0, 180000, 500),
    DECLARE_NSF_MELODY(crisis_force_nsf, "Crisis Force Stage 2", 1, 180000, 500),
    DECLARE_NSF_MELODY(castlevania3_nsf, "Castelvania Beginning", 0, 180000, 150),
    DECLARE_NSF_MELODY(contra_nsf, "Contra Jungle", 1, 180000, 200),
    DECLARE_NSF_MELODY(contra_nsf, "Contra Waterfall", 2, 180000, 250),
    DECLARE_NSF_MELODY(tinytoon_nsf, "Tiny Toon Theme 5", 4, 180000, 200),
    DECLARE_NSF_MELODY(tinytoon_nsf, "Tiny Toon Theme 9", 8, 180000, 200),
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
    if ( melodies[index].type == MELODY_TYPE_VGM || melodies[index].type == MELODY_TYPE_NSF )
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
