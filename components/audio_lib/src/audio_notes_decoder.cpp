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
}

void AudioNotesDecoder::set_format(uint32_t rate, uint8_t bps)
{
    m_rate = rate;
    m_bps = bps;
}

int AudioNotesDecoder::decode(uint8_t* origin_buffer, int max_size)
{
    if (m_melody == nullptr)
    {
        return -1;
    }
    uint8_t* buffer = origin_buffer;
    int remaining = 2048;
    while ( remaining > 0 )
    {
        if ( !m_note_samples_left && !m_pause_left )
        {
            if (!read_note_data()) break;
        }
        while ( (m_note_samples_left > 0) && (remaining > 0) )
        {
            uint16_t remainder = m_played_period % m_samples_per_period;
#if 1
            if ( remainder < (m_samples_per_period / 2) )
                m_last_decoded_level = 0xFFFF;
            else
                m_last_decoded_level = 0x0000;
#else
            if ( remainder < (m_samples_per_period / 2) )
                m_last_decoded_level = (0xFFFF * remainder) / (m_samples_per_period / 2);
            else
//                m_last_decoded_level = (0xFFFF);
                m_last_decoded_level = (0xFFFF * (m_samples_per_period - remainder)) / (m_samples_per_period / 2);
#endif
             // RIGHT ???
            *reinterpret_cast<uint16_t*>(buffer) = m_last_decoded_level;
            remaining -= (m_bps / 8);
            buffer += (m_bps / 8);
            // LEFT ???
            *reinterpret_cast<uint16_t*>(buffer) = m_last_decoded_level;
            remaining -= (m_bps / 8);
            buffer += (m_bps / 8);
            m_note_samples_left--;
            m_played_period++;
        }
        if ( m_note_samples_left == 0 )
        {
            while ( (m_pause_left > 0) && (remaining >0) )
            {
                *reinterpret_cast<uint16_t*>(buffer) = m_last_decoded_level;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
                *reinterpret_cast<uint16_t*>(buffer) = m_last_decoded_level;
                remaining -= (m_bps / 8);
                buffer += (m_bps / 8);
                m_pause_left--;
            }
            if ( m_pause_left == 0 )
            {
                next_note();
            }
        }
    }
    return buffer - origin_buffer;
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
//            m_note_samples_played = 0;
            m_samples_per_period = m_rate / note.freq;
            result = true;
            if ( m_melody->pause < 0 )
            {
                m_pause_left = m_rate * (m_note_samples_left  * (-m_melody->pause) / 32) / (1000);
            }
            else if ( m_melody->pause > 0)
            {
                m_pause_left = m_rate * m_melody->pause / 1000;
            }
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
//            m_note_samples_played = 0;
            m_samples_per_period = m_rate / note.freq;
            result = true;
            if ( m_melody->pause < 0 )
            {
                m_pause_left = m_rate * (note.duration * (-m_melody->pause) / 32) / (1000);
            }
            else if ( m_melody->pause > 0)
            {
                m_pause_left = m_rate * m_melody->pause / 1000;
            }
            break;
        }
        default:
            break;
    }
    if ( result )
    {
//        m_played_period = 0;
        m_played_period = (m_samples_per_period / 2) * m_last_decoded_level / 0xFFFF;
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

