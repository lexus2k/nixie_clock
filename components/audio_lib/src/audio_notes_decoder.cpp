#include "audio_notes_decoder.h"
#include <stdlib.h>

void AudioNotesDecoder::set_melody( NixieMelody* melody )
{
    m_melody = melody;
    m_position = melody->notes;
    if (m_buffer)
    {
        free(m_buffer);
    }
    m_buffer = static_cast<uint8_t*>(malloc(8192));
}

void AudioNotesDecoder::set_format(uint32_t rate, uint8_t bps)
{
    m_rate = rate;
    m_bps = bps;
    if (m_buffer)
    {
        free(m_buffer);
    }
    m_buffer = static_cast<uint8_t*>(malloc(8192));
}

int AudioNotesDecoder::decode()
{
    if (m_melody == nullptr)
    {
        return -1;
    }
    if ( m_melody->type == MELODY_TYPE_PROGMEM_TEMPO )
    {
        uint8_t* buffer = m_buffer;
        int remaining = 8192;
        for(;;)
        {
            SNixieTempoNote note = *reinterpret_cast<const SNixieTempoNote*>(m_position);
            if ( note.freq == NOTE_STOP ) break;
            int bytes_needed = m_rate / note.tempo;
            if (bytes_needed > remaining) break;
            remaining -= bytes_needed;
            while (bytes_needed > 0)
            {
                if ( (bytes_needed % note.freq) < (note.freq / 2) )
                    *buffer = 255;
                else
                    *buffer = 0;
                buffer++;
                bytes_needed--;
            }
            m_position += sizeof(SNixieTempoNote);
        }
        return buffer - m_buffer;
    }
    else if ( m_melody->type == MELODY_TYPE_PROGMEM_SAMPLING )
    {
        uint8_t* buffer = m_buffer;
        int remaining = 8192;
        for(;;)
        {
            SNixieSamplingNote note = *reinterpret_cast<const SNixieSamplingNote*>(m_position);
            if ( note.freq == NOTE_STOP ) break;
            int bytes_needed = m_rate / ( 1000000 / note.duration );
            if (bytes_needed > remaining) break;
            remaining -= bytes_needed;
            while (bytes_needed > 0)
            {
                if ( (bytes_needed % note.freq) < (note.freq / 2) )
                    *buffer = 255;
                else
                    *buffer = 0;
                buffer++;
                bytes_needed--;
            }
            m_position += sizeof(SNixieSamplingNote);
        }
        return buffer - m_buffer;
    }
    return 0;
}

uint8_t* AudioNotesDecoder::get_buffer()
{
    return m_buffer;
}

