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

#include "tiny_digital_buttons.h"
#include "esp_timer.h"
#include "esp_log.h"

#include <string.h>

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

static uint32_t get_diff(uint32_t curr_time, uint32_t last_time)
{
    return curr_time < last_time ? ((uint32_t)-1) - last_time + curr_time
                                 : curr_time - last_time;
}

TinyDigitalButtons::TinyDigitalButtons(const std::vector<button_desc_t>& btns)
{
    setup( btns );
}

void TinyDigitalButtons::setup(const std::vector<button_desc_t> &btns)
{
    m_buttons.clear();
    for (int i=0; i<btns.size(); i++)
    {
        button_info_t button_info{};
        button_info.desc = btns[i];
        button_info.index = i;
        m_buttons.emplace_back( button_info );
    }
}

TinyDigitalButtons::~TinyDigitalButtons()
{
    m_buttons.clear();
}

void TinyDigitalButtons::begin()
{
    for (int i=0; i< m_buttons.size(); i++)
    {
        // TODO: Put gpio setup here
    }
};

void TinyDigitalButtons::update()
{
    for (int i=0; i< m_buttons.size(); i++)
    {
        update_button(&m_buttons[i]);
    }
}
/**
 *  In milliseconds
 */
void TinyDigitalButtons::update_button(button_info_t * info)
{
    /* Do not sample too often */
    info->was_down = info->is_down;
    if ((0 == info->check_bounce) && (get_diff(millis(), info->last_timestamp_ms) < BUTTONS_SAMPLING_PERIOD))
    {
        return;
    }
    if ((info->check_bounce != 0) && (get_diff(millis(), info->last_timestamp_ms) < DEBOUNCE_SAMPLING_PERIOD))
    {
        return;
    }
    int value = gpio_get_level(static_cast<gpio_num_t>( info->desc.gpio ));
    if (value != info->last_value )
    {
        /* Read value differs from last rememberred. So, it seems that we need to perform
           debounce cycles for newly pressed button */
        info->check_bounce = 1;
    }
    info->last_timestamp_ms = millis();
    info->last_value = value;
    if (info->check_bounce < DEBOUNCE_CYCLES)
    {
        info->check_bounce++;
        return;
    }
    info->is_down = value == info->desc.active_level ? true: false;
    info->check_bounce = 0;
    if (get_diff(millis(), info->up_timestamp_ms) >= MIN_DELAY_AFTER_UP)
    {
        if ((info->is_down) && (!info->was_down))
        {
            info->down_timestamp_ms = millis();
            if (m_downHandler != nullptr)
            {
                m_downHandler(info->index, get_diff(millis(), info->up_timestamp_ms));
            }
        }
        else if ((info->is_down) && (info->was_down))
        {
            /* Otherwise, just holding button */
            if (m_holdHandler != nullptr)
            {
                m_holdHandler(info->index, get_diff(millis(), info->down_timestamp_ms));
            }
        }
        else if (info->was_down && !info->is_down)
        {
            if (info->disable_up)
            {
                info->was_down = false;
                info->disable_up = false;
            }
            else if ( m_upHandler != nullptr )
            {
                m_upHandler(info->index, get_diff(millis(), info->down_timestamp_ms));
            }
            info->up_timestamp_ms = millis();
        }
    }
}



