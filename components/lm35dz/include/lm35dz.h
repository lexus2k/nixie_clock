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

class Lm35Dz
{
public:
    Lm35Dz() = default;
    ~Lm35Dz() = default;

    void setup(adc1_channel_t channel, adc_bits_width_t width);

    void setup_peak_detector(int threshold);

    bool begin();
    void end();

    int get_raw() const;

    /** in hundreds of celsius degrees */
    int get_celsius_hundreds() const;

private:
    adc1_channel_t m_channel = ADC1_CHANNEL_MAX;
    int m_width = 4095;
};

