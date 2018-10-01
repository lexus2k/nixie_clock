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
    uint8_t* buffer = m_buffer;
    int remaining = 2048;
    while ( remaining > 0 )
    {
        if ( !m_note_samples_left && !m_pause_left )
        {
            if (!read_note_data()) break;
        }
        while ( (m_note_samples_left > 0) && (remaining > 0) )
        {
            uint16_t data;
            uint16_t remainder = m_note_samples_left % m_samples_per_period;
/*            if ( remainder < (m_samples_per_period / 2) )
                data = 0xFFFF;
            else
                data = 0x0000; */
            if ( remainder < (m_samples_per_period / 2) )
                data = (0xFFFF * remainder) / (m_samples_per_period / 2);
            else
                data = (0xFFFF * (remainder - (m_samples_per_period / 2))) / (m_samples_per_period / 2);
             // RIGHT ???
            *reinterpret_cast<uint16_t*>(buffer) = data;
            remaining -= (m_bps / 8);
            buffer += (m_bps / 8);
            // LEFT ???
            *reinterpret_cast<uint16_t*>(buffer) = data;
            remaining -= (m_bps / 8);
            buffer += (m_bps / 8);
            m_note_samples_left--;
        }
        if ( m_note_samples_left == 0 )
        {
            while ( (m_pause_left > 0) && (remaining >0) )
            {
                *reinterpret_cast<uint16_t*>(buffer) = 0x0000;
                m_pause_left--;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
            }
            if ( m_pause_left == 0 )
            {
                next_note();
            }
        }
    }
    return buffer - m_buffer;
}

uint8_t* AudioNotesDecoder::get_buffer()
{
    return m_buffer;
}

bool AudioNotesDecoder::read_note_data()
{
    bool result = false;
    
    switch ( m_melody->type )
    {
        case MELODY_TYPE_PROGMEM_TEMPO:
        {
            SNixieTempoNote note = *reinterpret_cast<const SNixieTempoNote*>(m_position);
            if ( note.freq == NOTE_STOP )
            {
                break;
            }
            if ( note.freq == NOTE_SILENT) note.freq = 1;
            m_note_samples_left = m_rate / note.tempo;
            m_samples_per_period = m_rate / note.freq;
            result = true;
            break;
        }
        case MELODY_TYPE_PROGMEM_SAMPLING:
        {
            SNixieSamplingNote note = *reinterpret_cast<const SNixieSamplingNote*>(m_position);
            if ( note.freq == NOTE_STOP )
            {
                break;
            }
            if ( note.freq == NOTE_SILENT) note.freq = 1;
            m_note_samples_left = (note.duration * m_rate) / 1000;
            m_samples_per_period = m_rate / note.freq;
            result = true;
            if ( m_melody->pause < 0 )
            {
                m_pause_left = m_rate * (note.duration * (-m_melody->pause) / 32) / (1000);
            }
            break;
        }
        default:
            break;
    }
    if ( result )
    {
        if ( m_melody->pause > 0 )
        {
            m_pause_left = m_rate * m_melody->pause / 1000;
        }
    }
    return result;
}

void AudioNotesDecoder::next_note()
{
    switch ( m_melody->type )
    {
        case MELODY_TYPE_PROGMEM_TEMPO:
            m_position += sizeof(SNixieTempoNote);
            break;
        case MELODY_TYPE_PROGMEM_SAMPLING:
            m_position += sizeof(SNixieSamplingNote);
            break;
        default:
            break;
    }
}

