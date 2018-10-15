/*
    Copyright (C) 2016-2018 Alexey Dynda

    This file is part of Nixie Os Library.

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

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <stdint.h>

#define TASK_ID_INVALID 0xFF

#define NIXIEOS_TASK(id, x)  { id, \
                               &x ## _on_enter, \
                               &x ## _main, \
                               &x ## _on_event, \
                               &x ## _on_exit }

#define NIXIEOS_TASK_END     { TASK_ID_INVALID, nullptr, nullptr, nullptr, nullptr }


/**
 * Describes single state of NixieOs
 */
typedef struct
{
    /// unique id of the state
    uint8_t id;
    /// state entry function
    void  (*enter_cb)();
    /// state main function
    void  (*state_cb)();
    /// state event processing function
    void  (*event_cb)(uint8_t event, uint8_t arg);
    /// state exit function
    void  (*exit_cb)();
} state_info_t;

class SmEngine
{
public:
    SmEngine(const state_info_t *states);
    ~SmEngine() = default;

    /**
     * @brief Starts infinite loop.
     *
     * Enters infinite loop and runs state machine. If you want
     * to do anything else outside state machine functions, please,
     * do not use this function. Refer to run() function instead.
     *
     */
    void loop();

    /**
     * @brief Runs single iteration of state machine.
     *
     * Runs single iteration of state machine and exists.
     */
    void update();

    /**
     * @brief remembers current state and switches to new state.
     *
     * Remembers current state number in the NixieOs stack and
     * switches to new state. This function calls SSSEnterFunction(),
     * where SSS is state name.
     *
     * @param id - state unique number.
     *
     * @see pop_state
     */
    void push_state(uint8_t id);

    /**
     * @brief switches back to remembered state.
     *
     * Switches to remembered state. This function calls SSSEnterFunction(),
     * where SSS is state name.
     *
     * @see push_state
     */
    void pop_state();

    /**
     * @brief switches to new state.
     *
     * Switches to new state. This function calls SSSEnterFunction(),
     * where SSS is state name.
     *
     * @param id - state unique number.
     */
    void switch_state(uint8_t id);

    /**
     * @brief puts new event to internal NixieOs queue.
     *
     * Puts new event to internal NixieOs queue. The events
     * from internal queue are peeked by run() method and
     * are passed to SSSEventFunction() if the currently
     * active state.
     *
     * @param id - event unique id
     * @param param - additional parameter for the event.
     * @return true if event is put to queue
     *         false if there is no space in internal event queue.
     */
    bool send_event(uint8_t id, uint8_t param);

private:

    const state_info_t *m_states;
    state_info_t m_active_state;
    QueueHandle_t m_queue;
    bool m_started = false;
    uint8_t m_pop_state;
};
