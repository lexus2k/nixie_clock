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

#include "tiny_buttons.h"
#include <stdint.h>
#include "driver/adc.h"

typedef struct
{
    int      gpio;
    int      last_value;
    uint32_t last_timestamp_ms;
    uint32_t down_timestamp_ms;
    uint32_t up_timestamp_ms;
    bool     is_down;
    bool     was_down;
    bool     disable_up;
    int      check_bounce;
    int      index;
} button_info_t;

/**
 * Class to process ADC line, which ADC buttons are connected to.
 */
class TinyDigitalButtons
{
public:
    /**
     * Creates Buttons class.
     * The constructor accepts analog pin number to use and array of button ADC values, expected
     * on the pin
     * @param btns - array of gpio pin numbers, corresponding to pressed buttons
     * @param count - number of buttons described in btns array.
     */
    TinyDigitalButtons(const int16_t btns[], uint8_t count);

    ~TinyDigitalButtons();

    /**
     * Initializes state of the object. Must be called in setup() function.
     */
    void begin();

    /**
     * stops all
     */
    void end();

    /**
     * Disables Up action if button is pressed now.
     * Useful to prevent button release action, when we need to process hold state only.
     */
    inline void disableUpAction(int n) { if (m_buttons[n].is_down) m_buttons[n].disable_up = true; };

    /**
     * Updates state of Buttons class, must be called in the cycle each time.
     */
    void update                        ();

    /**
     * Sets handler for button down events. Handler should accept button id
     * and time delta in milliseconds as the only parameters.
     * @param[in] handler - callback handler of TOnButtonEvent type
     */
    void onButtonDown(TOnButtonEvent handler) { m_downHandler = handler; };

    /**
     * Sets handler for button up events. Handler should accept button id
     * and time delta in milliseconds as the only parameters.
     * @param[in] handler - callback handler of TOnButtonEvent type
     */
    void onButtonUp(TOnButtonEvent handler)   { m_upHandler   = handler; };


    /**
     * Sets handler for button hold events. Handler should accept button id
     * and time delta in milliseconds as the only parameters.
     * @param[in] handler - callback handler of TOnButtonEvent type
     */
    void onButtonHold(TOnButtonEvent handler)   { m_holdHandler   = handler; };

private:
    button_info_t * m_buttons = nullptr;
    uint8_t  m_count;

    uint32_t m_lastEventTimestampMs; // last timestamp in milliseconds

    TOnButtonEvent  m_downHandler       = nullptr;
    TOnButtonEvent  m_upHandler         = nullptr;
    TOnButtonEvent  m_holdHandler       = nullptr;

    void update_button(button_info_t * info);
};

