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

#pragma once

#include <stdint.h>
#include "driver/adc.h"
#include <vector>

class Gl5528
{
public:
    Gl5528() = default;
    ~Gl5528() = default;

    void setup(adc1_channel_t channel);

    bool begin();
    void update();
    void end();

    int get_raw();
    int get_raw_avg();

    /**
     * Returns adc value in range 0% - 100%, if no adc data returns -1
     */
    int get();

    /**
     * Returns average adc value in range 0% - 100%, if no adc data returns -1
     */
    int get_avg();

    bool is_peak_detected(uint32_t min_duration_ms, uint32_t max_duration_ms);


private:
    adc1_channel_t m_channel = ADC1_CHANNEL_MAX;
    const int m_width = 4095;
    uint32_t m_accum = 0;
    int m_count = 0;
    bool m_peek_detected = false;
    bool m_deviation_detected = false;
    uint32_t m_peek_start_ms = 0;
    uint32_t m_peek_end_ms = 0;
};

