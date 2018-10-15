#pragma once

#include "audio_i2s.h"
#include "audio_notes_decoder.h"

class AudioPlayer
{
public:
    AudioPlayer() = default;
    ~AudioPlayer() = default;

    void play(const NixieMelody* melody);
    void begin();
    void end();
    /**
     * Return false, when nothing is played
     */
    bool update();

private:
    AudioI2S m_output;
    AudioDecoder* m_decoder = nullptr;
    uint8_t* m_buffer = nullptr;
    uint8_t* m_player_pos = nullptr;
    size_t  m_size = 0;
};