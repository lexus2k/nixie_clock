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
     * Calls begin() method of all state machine states
     */
    bool begin();

    /**
     * Calls end() method of all state machine states
     */
    void end();

    /**
     * @brief Starts infinite loop.
     *
     * Enters infinite loop and runs state machine. If you want
     * to do anything else outside state machine functions, please,
     * do not use this function. Refer to run() function instead.
     *
     * @param event_wait_timeout_ms timeout to wait for incoming event. Do not use 0,
     *        since loop() will occupy 100% cpu.
     */
    void loop(uint32_t event_wait_timeout_ms);

    /**
     * @brief Runs single iteration of state machine.
     *
     * Runs single iteration of state machine and exits.
     *
     * @param event_wait_timeout_ms timeout to wait for incoming event. If 0, the function
     *        will return immediately.
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

    /**
     * @brief change current state to new one
     *
     * Changes current state to new one. For current state method exit()
     * will be called, for new state method enter() will be called.
     *
     * @param new_state id of new state to switch to
     */
    bool switch_state(uint8_t new_state);

    /**
     * @brief change current state to new one, but stores current state
     *
     * Changes current state to new one, but stores current state id.
     * For current state method exit() will be called, for new state method
     * enter() will be called. To return to stored state use pop_state() method.
     *
     * @param new_state id of new state to switch to
     */
    bool push_state(uint8_t new_state);

    /**
     * @brief returns to last stored state.
     *
     * returns to last stored state.
     * @see push_state
     */
    bool pop_state();

    /**
     * @brief registers new state in state machine memory
     *
     * Registers new state in state machine memory
     *
     * @param state reference to SmState-based object
     */
    void add_state(SmState &state);

    /**
     * @brief registers new state in state machine memory
     *
     * Registers new state in state machine memory. SmState-based
     * object will be automatically allocated and freed by state machine
     *
     */
    template <class T>
    void add_state()
    {
        T *p = new T();
        register_state( *p, true );
    }

    /**
     * Terminates state machine. This causes loop() method to exit.
     */
    void stop() { m_stopped = true; }

    uint8_t get_state_id();

protected:

    /**
     * The method can be used to perform additional initialization
     * before any state objects starts to work
     */
    virtual bool on_begin();

    /**
     * The method can be used to hook all events for all states
     */
    virtual EEventResult on_event(SEventData event);

    /**
     * This method can be used to perform actions in all states
     */
    virtual void on_update();

    /**
     * The method is called after all states are ended
     */
    virtual void on_end();

private:
    typedef struct
    {
        SmState *state;
        bool auto_allocated;
    } SmStateInfo;

    std::stack<SmState*> m_stack;
    SmState *m_active = nullptr;
    QueueHandle_t m_queue;
    std::list<__SDeferredEventData> m_events;
    std::list<SmStateInfo> m_states;
    bool m_stopped = false;

    EEventResult process_event(SEventData &event);
    void register_state(SmState &state, bool auto_allocated);
};

