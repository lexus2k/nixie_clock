/*
    Copyright (C) 2020 Alexey Dynda

    This file is part of State Machine Engine library.

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
#include "sme_config.h"

#define SM_EVENT_TIMEOUT   0xFF

#define SM_STATE_NONE      0xFF

#define SM_EVENT_ARG_ANY   UINTPTR_MAX

enum class EEventResult: uint8_t
{
    NOT_PROCESSED,
    PROCESSED,
    PROCESSED_AND_HOOKED,
};

typedef struct
{
    uint8_t event;
    uintptr_t arg;
} SEventData;

typedef struct
{
    SEventData event;
    uint32_t micros;
} __SDeferredEventData;

class ISmeState
{
public:
    ISmeState(const char *name): m_name( name ) { }

    virtual ~ISmeState() = default;

    /**
     * State initialization (is called, when new state is being added to state machine)
     */
    virtual bool begin() { return true; }

    /**
     * State deinitialization (is called, when state machine is destroyed)
     */
    virtual void end() { }

    /**
     * enter function is called, when state is being activated
     */
    virtual void enter() { }

    /**
     * update() is called in loop, when state is active
     */
    virtual void update() { }

    /**
     * exit function is called, when state is being deactivated.
     */
    virtual void exit() { }

    /**
     * on_event method is called, when new event arrives in active state
     */
    virtual EEventResult on_event(SEventData event) { return EEventResult::NOT_PROCESSED; }

    /**
     * Returns state id
     */
    uint8_t get_id() { return m_id; }

    /**
     * Returns state name
     */
    const char *get_name() { return m_name; }

    /**
     * Sets state id. Id is changed only if it was not previously specified for the state.
     */
    void set_id(uint8_t id) { if ( m_id == SM_STATE_NONE ) m_id = id; }

    void set_parent( ISmeState * parent ) { m_parent = parent; }

protected:

    /**
     * Initiates switching to new state. The method doesn't guarantuee immediate state switch
     * @param new_state state to switch to.
     * @return true if successful
     */
    virtual bool switch_state(uint8_t new_state) { return m_parent ? m_parent->switch_state( new_state ) : false; }

    /**
     * Returns from current state to state saved to stack
     */
    virtual bool pop_state() { return m_parent ? m_parent->pop_state() : false; }

    /**
     * Switches to new state and remembers current state in stack
     * @param new_state state to switch to.
     * @return true if successful
     */
    virtual bool push_state(uint8_t new_state) { return m_parent ? m_parent->push_state( new_state ) : false; }

    /**
     * @brief sends event state machine event queue
     *
     * Sends event to state machine event queue
     *
     * @param event event to put to queue
     */
    virtual bool send_event(SEventData event) { return m_parent ? m_parent->send_event( event ) : false; }

    /**
     * Returns timestamp in microseconds, since system is up
     */
//    uint64_t get_micros();

    /**
     * Returns true, when timeout takes place, and sends timeout event to queue
     * if generate_event is set to true
     * @param timeout timeout in microseconds
     * @param generate_event true if state machine timeout event should be generated
     * @return true if timeout, false otherwise
     */
    virtual bool timeout_event(uint64_t timeout, bool generate_event = false)
    {
        return m_parent ? m_parent->timeout_event( timeout, generate_event ) : false;
    }

    /**
     * Resets internal state timer
     */
    virtual void reset_timeout() { if (m_parent) m_parent->reset_timeout(); }

    void force_set_id(uint8_t id) { m_id = id; }

private:

    uint8_t m_id = SM_STATE_NONE;

    const char * m_name;

    ISmeState * m_parent = nullptr;
};

