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
    int get();
    int get_avg();


private:
    adc1_channel_t m_channel;
    const int m_width = 4095;
};

#if 0

#define TINY_BUTTON_SHORT_PRESS_MS    400

#define TINY_BUTTON_ANY   0xFF

/**
 * TOnButtonEvent defines format of callback functions used by TinyAnalogButtons class
 * @param id           - id of the pressed button
 * @param timeDeltaMs  - delta in milliseconds
 *                For button down events:  delta means time passed since up event
 *                For button up events:    delta means time passed since down event
 *                For button hold events:  delta means time passed since down event,
 *                                         when button is not released yet.
 */
typedef void (*TOnButtonEvent)(uint8_t id, uint16_t timeDeltaMs);

#ifndef ANALOG_BUTTONS_THRESHOLD
    #define ANALOG_BUTTONS_THRESHOLD  40
#endif

/**
 * Class to process ADC line, which ADC buttons are connected to.
 */
class TinyAnalogButtons
{
public:
    TinyAnalogButtons() {};

    /**
     * Creates Buttons class.
     * The constructor accepts analog pin number to use and array of button ADC values, expected
     * on the pin
     * @param adcPin - pin, which the buttons are connected to
     * @param btns - array of ADC values, corresponding to pressed buttons
     */
    TinyAnalogButtons(adc1_channel_t channel, const std::vector<int16_t>& btns)
    {
        setup(channel, btns);
    };

    /**
     * The method accepts analog pin number to use and array of button ADC values, expected
     * on the pin
     * @param adcPin - pin, which the buttons are connected to
     * @param btns - array of ADC values, corresponding to pressed buttons
     */
    void setup(adc1_channel_t channel, const std::vector<int16_t>& btns)
    {
        m_channel = channel;
        m_buttons = btns;
    }

    /**
     * Initializes state of the object. Must be called in setup() function.
     */
    void begin();

    /**
     * stops all
     */
    void end();

    /**
     * Returns true if button is being up, and press duration is below 300 milliseconds
     * @param n - button id to check
     * @return true or false
     */
    inline bool isShortPress           (uint8_t n)   { return isShortPress(n, TINY_BUTTON_SHORT_PRESS_MS); };

    /**
     * Returns true if any button or their combination is pressed.
     * @returns true or false
     */
    inline bool isShortPress           ()         { return (m_isButtonDown == false) && (m_wasButtonDown == true) && holdDuration() < TINY_BUTTON_SHORT_PRESS_MS; };

    /**
     * Returns true if button is being up, and press duration is no longer than duration value
     * @param n - button id to check
     * @param duration - maximum allowed duration in milliseconds
     */
    inline bool isShortPress           (uint8_t n, uint16_t duration)  { return isButtonUp(n) && holdDuration() < duration; };

    /**
     * Returns true if button is being up, and press duration is longer than 1000 milliseconds.
     * @param n - button id to check
     * @return true or false
     */
    inline bool isLongPress            (uint8_t n)   { return isLongPress(n, 1000); };

    /**
     * Returns true if button is being hold more than 1 second.
     * @param n - button id to check
     * @param ts - time, returned by millis() function
     * @return true or false
     */
    inline bool isLongHold             (uint8_t n, uint16_t ts) { return isLongHold(n, ts, 1000); }

    /**
     * Returns true if button is being hold more than 1 second.
     * @param n - button id to check
     * @param ts - time, returned by millis() function
     * @param duration - minimum hold threshold duration in milliseconds
     * @return true or false
     */
    inline bool isLongHold             (uint8_t n, uint16_t ts, uint16_t duration)
                                               {
                                                    if (holdDuration(n, ts) > duration)
                                                    {
                                                        disableUpAction();
                                                        return true;
                                                    }
                                                    return false;
                                                }


    /**
     * Returns true if button is being up, and press duration is longer than specified duration value
     * @param n - button id to check
     * @param duration - minimum duration, while button should stay pressed
     * @return true or false
     */
    inline bool isLongPress            (uint8_t n, uint16_t duration)  { return isButtonUp(n) && holdDuration() > duration; };

    /**
     * Returns true if button is being down
     */
    inline bool isButtonDown           (uint8_t n)   { return (m_id == n) && (m_isButtonDown == true); };

    inline bool isButtonDown           ()            { return m_isButtonDown; };

    /**
     * Returns true if button is being up
     */
    inline bool isButtonUp             (uint8_t n)   { return (m_id == n) && (m_isButtonDown == false) && (m_wasButtonDown == true); };

    /**
     * Returns true whever there are all buttons are up now
     */
    inline bool isNoAction             ( )        { return !m_isButtonDown && !m_wasButtonDown; };

    /**
     * Returns number of milliseconds passed between button down and up events.
     * To find the button id, which this value is valid for, call isButtonUp method.
     */
    inline uint16_t holdDuration       ()         { return m_upTimestampMs - m_downTimestampMs; };

    /**
     * Disables Up action if button is pressed now.
     * Useful to prevent button release action, when we need to process hold state only.
     */
    inline void disableUpAction() { if (m_isButtonDown) m_disableAction = true; };

    /**
     * Returns number of milliseconds passed between button down event and specified time in milliseconds.
     * @param n - button id to check
     * @param ts - time, returned by millis() function
     * @return 0 - if n doesn't correspond to button id, being held
     *         number of milliseconds between button down event and specified time for N button.
     */
    inline uint16_t holdDuration       (uint8_t n, uint16_t ts)
    {
        return ((m_id == n) && (m_isButtonDown == true) && (m_disableAction == false)) ? (ts - m_downTimestampMs) : 0;
    };

    inline void reset() { m_isButtonDown = false; m_wasButtonDown = false; };

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

    uint8_t getButtonId() { return m_id; };

    int get_count() { return m_buttons.size(); }

private:
    uint8_t  m_id                       = 0xFF;
    std::vector<int16_t>                m_buttons;
    uint32_t m_downTimestampMs          = 0;
    uint32_t m_upTimestampMs            = 0;
    bool     m_isButtonDown             = false;
    bool     m_wasButtonDown            = false;
    bool     m_disableAction            = false;
    adc1_channel_t  m_channel;
    uint32_t m_lastEventTimestampMs; // last timestamp in milliseconds
    int      m_lastReadAdc;
    uint8_t  m_checkBounce              = 0;
    TOnButtonEvent  m_downHandler       = nullptr;
    TOnButtonEvent  m_upHandler         = nullptr;
    TOnButtonEvent  m_holdHandler       = nullptr;
};

#endif
