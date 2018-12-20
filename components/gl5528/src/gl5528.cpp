/*
    Copyright (C) 2017-2018 Alexey Dynda

    This file is part of TinyButtons Library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gl5528.h"
#include "esp_timer.h"
#include "esp_log.h"

static uint32_t millis()
{
    return esp_timer_get_time() / 1000;
}

void Gl5528::setup(adc1_channel_t channel)
{
    m_channel = channel;
}

bool Gl5528::begin()
{
    if ( m_channel == ADC1_CHANNEL_MAX )
    {
        return false;
    }
    // Up to 3.9V
    return adc1_config_channel_atten( m_channel, ADC_ATTEN_DB_11 ) == ESP_OK;
}

void Gl5528::update()
{
    if ( m_channel == ADC1_CHANNEL_MAX)
    {
        return;
    }
    int value = get_raw();
    m_accum += value;
    m_count++;
    fprintf(stderr, "AVG:%d, ALS: %d\n", get_raw_avg(), value);
    if (m_count > 100)
    {
        m_accum -= get_raw_avg();
        m_count--;
        if ( value < (get_raw_avg() - (m_width >> 7)) )
        {
            if (m_peek_detected)
            {
                m_peek_end = millis();
            }
            else
            {
                m_peek_start = millis();
                m_peek_end = m_peek_start;
                m_peek_detected = true;
            }
        }
        else
        {
            m_peek_detected = false;
        }
    }
}

void Gl5528::end()
{
}

int Gl5528::get_raw()
{
    return adc1_get_raw( m_channel );
}

int Gl5528::get_raw_avg()
{
    return m_accum / m_count;
}

int Gl5528::get()
{
    return m_channel*100 / m_width;
}

int Gl5528::get_avg()
{
    return 0;
}

bool Gl5528::is_peak_detected(uint32_t duration_ms)
{
    if (m_peek_detected)
    {
        return false;
    }
    if (m_peek_start != m_peek_end)
    {
        if ( static_cast<uint32_t>(m_peek_end - m_peek_start) < duration_ms  )
        {
            m_peek_start = m_peek_end;
            return true;
        }
    }
    return false;
}

#if 0

#define BUTTONS_SAMPLING_PERIOD   20 /// period in milliseconds
#define DEBOUNCE_SAMPLING_PERIOD  5  /// period in milliseconds
#define DEBOUNCE_CYCLES           3  /// 3*5 = 15 milliseconds
#define MIN_DELAY_AFTER_UP        50 /// minimum delay before the button can be pressed again
#define MAX_WAIT_DOUBLE_PRESS_TIMEOUT  \
                                  300

static uint32_t millis()
{
    return esp_timer_get_time() / 1000;
}

void TinyAnalogButtons::begin()
{
    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(m_channel,ADC_ATTEN_DB_11); // Up to 3.9V
};

static uint32_t get_diff(uint32_t curr_time, uint32_t last_time)
{
    return curr_time < last_time ? ((uint32_t)-1) - last_time + curr_time
                                 : curr_time - last_time;
}

/**
 *  In milliseconds
 */
void TinyAnalogButtons::update()
{
    /* Do not sample too often */
    m_wasButtonDown = m_isButtonDown;
    if ((0 == m_checkBounce) && (get_diff(millis(), m_lastEventTimestampMs) < BUTTONS_SAMPLING_PERIOD))
    {
        return;
    }
    if ((m_checkBounce != 0) && (get_diff(millis(), m_lastEventTimestampMs) < DEBOUNCE_SAMPLING_PERIOD))
    {
        return;
    }
    int value = adc1_get_raw( m_channel );
//    if (value == ADC_IN_PROGRESS) return;
    if ((value > m_lastReadAdc + ANALOG_BUTTONS_THRESHOLD) || (value < m_lastReadAdc - ANALOG_BUTTONS_THRESHOLD))
    {
        /* Read value differs from last rememberred. So, it seems that we need to perform
           debounce cycles for newly pressed button */
        m_checkBounce = 1;
    }
    m_lastEventTimestampMs = millis();
    m_lastReadAdc = value;
    if (m_checkBounce < DEBOUNCE_CYCLES)
    {
        m_checkBounce++;
        return;
    }
    m_checkBounce = 0;
    bool     found = false;
    for(uint8_t n = 0; n < m_buttons.size(); n++)
    {
        found = (value < m_buttons[n] + ANALOG_BUTTONS_THRESHOLD) &&
                (value > m_buttons[n] - ANALOG_BUTTONS_THRESHOLD);
        if (found && (get_diff(millis(), m_upTimestampMs) >= MIN_DELAY_AFTER_UP))
        {
            // Change only, if new button is pressed and no much time passed since last hit
            if ( ((m_id != n) && (get_diff(millis(), m_downTimestampMs) < MAX_WAIT_DOUBLE_PRESS_TIMEOUT))
                ||
                // or if button is being pressed
                (m_isButtonDown == false))
            {
                m_id = n;
                m_isButtonDown = true;
                m_downTimestampMs = millis(); 
                if (m_downHandler != nullptr)
                {
                    m_downHandler(m_id, get_diff(millis(), m_upTimestampMs));
                }
            }
            else
            {
                /* Otherwise, just holding button */
                if (m_holdHandler != nullptr)
                {
                    m_holdHandler(m_id, get_diff(millis(), m_downTimestampMs));
                }
            }
        }
        if (found)
        {
            break;
        }
    }
    /* If no pressed button is found, then all buttons are up */
    if (!found)
    {
        if (m_isButtonDown)
        {
            m_isButtonDown = false;
            if (m_disableAction == true)
            {
                m_wasButtonDown = false;
                m_disableAction = false;
            }
            else if ( m_upHandler != nullptr )
            {
                m_upHandler(m_id, get_diff(millis(), m_downTimestampMs));
            }
            m_upTimestampMs = millis(); 
        }
    }
}


#endif
