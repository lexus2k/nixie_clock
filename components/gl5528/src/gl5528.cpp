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

void Gl5528::setup(adc1_channel_t channel, adc_bits_width_t width)
{
    m_channel = channel;
    m_width = (1 << (static_cast<int>(width) + 9)) - 1;
}

void Gl5528::setup_peak_detector(int threshold)
{
    m_peak_threshold = threshold ? threshold : (m_width / 100);
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
        if (m_deviation_detected)
        {
            if (m_peak_value > value)
            {
                m_peak_value = value;
            }
            m_peak_end_ms = millis();
            if (value > m_normal_value - m_peak_threshold)
            {
                m_deviation_detected = false;
                m_peak_detected = true;
            }
        }
        else if ( value < (get_raw_avg() - m_peak_threshold) )
        {
            m_peak_start_ms = millis();
            m_peak_end_ms = m_peak_start_ms;
            m_deviation_detected = true;
            m_peak_detected = false;
            m_peak_value = value;
            m_normal_value = get_raw_avg();
        }
    }
}

void Gl5528::end()
{
}

int Gl5528::get_raw() const
{
    return m_channel == ADC1_CHANNEL_MAX ? -1 : adc1_get_raw( m_channel );
}

int Gl5528::get_raw_avg() const
{
    if ( m_count == 0 )
    {
         return -1;
    }
    return m_accum / m_count;
}

int Gl5528::get() const
{
    return get_raw() * 100 / m_width;
}

int Gl5528::get_avg() const
{
    int value = get_raw_avg();
    if (value < 0)
    {
        return -1;
    }
    return value * 100 / m_width;
}

bool Gl5528::is_peak_detected(uint32_t min_duration_ms, uint32_t max_duration_ms) const
{
    if (m_peak_detected)
    {
        uint32_t duration = get_peak_duration();
        if ( duration > min_duration_ms && duration < max_duration_ms )
        {
            return true;
        }
    }
    return false;
}

bool Gl5528::is_peak_detected() const
{
    return m_peak_detected;
}

int Gl5528::get_peak_deviation() const
{
    return get_raw_avg() - m_peak_value;
}

uint32_t Gl5528::get_peak_duration() const
{
    return static_cast<uint32_t>(m_peak_end_ms - m_peak_start_ms);
}

void Gl5528::reset_peak_detector()
{
    m_peak_detected = false;
    m_deviation_detected = false;
}
