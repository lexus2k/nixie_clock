/*
    Copyright (C) 2016-2019 Alexey Dynda

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

#include "sm_state.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <stack>
#include <list>
#include <stdint.h>

class SmEngine2
{
public:
    SmEngine2(int max_queue_size = 10);
    ~SmEngine2();

    /**
     *
     */
    bool begin();

    /**
     *
     */
    void end();

    /**
     * @brief Starts infinite loop.
     *
     * Enters infinite loop and runs state machine. If you want
     * to do anything else outside state machine functions, please,
     * do not use this function. Refer to run() function instead.
     *
     */
    void loop(uint32_t event_wait_timeout_ms);

    /**
     * @brief Runs single iteration of state machine.
     *
     * Runs single iteration of state machine and exists.
     */
    bool update(uint32_t event_wait_timeout_ms);

    /**
     * @brief sends event state machine event queue
     *
     * Sends event to state machine event queue
     *
     * @param event event to put to queue
     */
    bool send_event(SEventData event);

    /**
     * @brief sends event state machine event queue after ms timeout
     *
     * Sends event to state machine event queue after ms timeout
     *
     * @param event event to put to queue
     * @param ms timeout in milliseconds
     */
    bool send_delayed_event(SEventData event, uint32_t ms);

    bool switch_state(uint8_t new_state);

    bool push_state(uint8_t new_state);

    bool pop_state();

    void add_state(SmState &state);

    template <class T>
    void add_state()
    {
        T *p = new T();
        p->m_sm_owner = true;
        add_state( *p );
    }

    void stop() { m_stopped = true; }


protected:

    /**
     * The method can be used to perform additional initialization
     * before any state objects starts to work
     */
    virtual bool on_begin();

    /**
     * The method can be used to hook all events for all states
     */
    virtual bool on_event(SEventData event);

    /**
     * This method can be used to perform actions in all states
     */
    virtual void on_update();

    /**
     * The method is called after all states are ended
     */
    virtual void on_end();

private:
    std::stack<SmState*> m_stack;
    SmState *m_first = nullptr;
    SmState *m_active = nullptr;
    QueueHandle_t m_queue;
    std::list<__SDeferredEventData> m_events;
    bool m_stopped = false;

    bool process_event(SEventData &event);
};

