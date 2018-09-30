#pragma once

#include "nixie_audio_defs.h"
#include <stdint.h>

class AudioNotesDecoder
{
public:
    AudioNotesDecoder() = default;
    ~AudioNotesDecoder() = default;

    void set_format(uint32_t rate, uint8_t bps);
    void set_melody( const NixieMelody* melody );

    int decode();
    uint8_t* get_buffer();

private:
    const NixieMelody* m_melody = nullptr;
    uint32_t m_rate = 8000;
    uint8_t m_bps = 16;
    const uint8_t *m_position = nullptr;
    uint16_t m_note_samples_left = 0;
    uint16_t m_pause_left = 0;
    uint8_t *m_buffer = nullptr;
};
