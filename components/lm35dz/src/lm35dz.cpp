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

#include "lm35dz.h"
#include "esp_timer.h"
#include "esp_log.h"

constexpr uint8_t READOUTS_AVERAGING = 8;

void Lm35Dz::setup(adc1_channel_t channel, adc_bits_width_t width)
{
    m_channel = channel;
    m_width = (1 << (static_cast<int>(width) + 9)) - 1;
}

bool Lm35Dz::begin()
{
    if ( m_channel == ADC1_CHANNEL_MAX )
    {
        return false;
    }
    // Up to 1.1V
    return adc1_config_channel_atten( m_channel, ADC_ATTEN_DB_0 ) == ESP_OK;
}

void Lm35Dz::end()
{
}

int Lm35Dz::get_raw() const
{
    return adc1_get_raw( m_channel );
}

int Lm35Dz::get_celsius_hundreds()
{
    int32_t raw = get_raw();
    // tempC = Voltage (mV) * 0.1;
    // tempC in Hundreeds = mV * 10; V = refVoltage * value / adc_width
    if ( m_readouts_count >= READOUTS_AVERAGING )
    {
        m_last_readouts -= (m_last_readouts / m_readouts_count);
    }
    else
    {
        m_readouts_count++;
    }
    m_last_readouts += (raw * 11000 / m_width);

    return m_last_readouts / m_readouts_count;
}

