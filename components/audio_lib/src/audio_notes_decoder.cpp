#include "audio_notes_decoder.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void AudioNotesDecoder::set_melody( const NixieMelody* melody )
{
    m_melody = melody;
    m_position = melody->notes;
    m_note_samples_left = 0;
    m_pause_left = 0;
    if (m_buffer)
    {
        free(m_buffer);
    }
    m_buffer = static_cast<uint8_t*>(malloc(2048));
}

void AudioNotesDecoder::set_format(uint32_t rate, uint8_t bps)
{
    m_rate = rate;
    m_bps = bps;
    if (m_buffer)
    {
        free(m_buffer);
    }
    m_buffer = static_cast<uint8_t*>(malloc(2048));
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
        int remaining = 2048;
        while ( remaining > 0 )
        {
            SNixieTempoNote note = *reinterpret_cast<const SNixieTempoNote*>(m_position);
            if ( note.freq == NOTE_STOP ) break;
            if ( note.freq == NOTE_SILENT) note.freq = 1;
            int samples = m_rate / note.tempo;
            if ( m_note_samples_left > 0) samples = m_note_samples_left;
            if ( m_pause_left > 0 ) samples = 0;
            int samples_per_period = m_rate / note.freq;
            while ( (samples > 0) && (remaining > 0) )
            {
                uint16_t data;
                uint16_t remainder = samples % samples_per_period;
                if ( remainder < (samples_per_period / 2) )
                    data = 0xFFFF;
                else
                    data = 0x0000;
                *reinterpret_cast<uint16_t*>(buffer) = data;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
                *reinterpret_cast<uint16_t*>(buffer) = 0x0000;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
                samples--;
            }
            m_note_samples_left = samples;
            if ( samples == 0 )
            {
                if ( m_pause_left == 0)
                {
                    m_pause_left = m_rate * m_melody->pause / 1000;
                }
                while ( (m_pause_left > 0) && (remaining >0) )
                {
                    *reinterpret_cast<uint16_t*>(buffer) = 0x0000;
                    m_pause_left--;
                    remaining -= (m_bps / 8);
                    buffer += (m_bps / 8);
                }
                if ( m_pause_left == 0 )
                {
                    m_position += sizeof(SNixieTempoNote);
                }
            }
        }
        return buffer - m_buffer;
    }
    else if ( m_melody->type == MELODY_TYPE_PROGMEM_SAMPLING )
    {
        uint8_t* buffer = m_buffer;
        int remaining = 2048;
        memset( buffer, 0, 2048 );
        while ( remaining > 0 )
        {
            SNixieSamplingNote note = *reinterpret_cast<const SNixieSamplingNote*>(m_position);
            if ( note.freq == NOTE_STOP ) break;
            if ( note.freq == NOTE_SILENT) note.freq = 1;
            int samples = (note.duration * m_rate) / 1000;
            if ( m_note_samples_left > 0) samples = m_note_samples_left;
            if ( m_pause_left > 0 ) samples = 0;
            int samples_per_period = m_rate / note.freq;
            while ( (samples > 0) && (remaining > 0) )
            {
                uint16_t data;
                uint16_t remainder = samples % samples_per_period;
/*                if ( remainder < (samples_per_period / 2) )
                    data = 0xFFFF;
                else
                    data = 0x0000;*/
                if ( remainder < (samples_per_period / 2) )
                    data = (0xFFFF * remainder) / (samples_per_period / 2);
                else
                    data = (0xFFFF * (remainder - (samples_per_period / 2))) / (samples_per_period / 2) ;
                *reinterpret_cast<uint16_t*>(buffer) = data;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
                *reinterpret_cast<uint16_t*>(buffer) = 0x0000;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
                samples--;
            }
            m_note_samples_left = samples;
            if ( samples == 0 )
            {
                if ( m_pause_left == 0)
                {
                    if ( m_melody->pause < 0 )
                        m_pause_left = m_rate * (note.duration * (-m_melody->pause) / 32) / (1000);
                    else
                        m_pause_left = m_rate * m_melody->pause / 1000;
                }
                while ( (m_pause_left > 0) && (remaining >0) )
                {
                    *reinterpret_cast<uint16_t*>(buffer) = 0x0000;
                    m_pause_left--;
                    remaining -= (m_bps / 8);
                    buffer += (m_bps / 8);
                }
                if ( m_pause_left == 0 )
                {
                    m_position += sizeof(SNixieSamplingNote);
                }
            }
        }
        return buffer - m_buffer;
    }
    return 0;
}

uint8_t* AudioNotesDecoder::get_buffer()
{
    return m_buffer;
}

