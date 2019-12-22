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

#include <stdint.h>
#include <freertos/FreeRTOS.h>

class SmEngine2;

#define SM_EVENT_TIMEOUT   0xFF
#define SM_EVENT_ARG_NONE  UINTPTR_MAX

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
    TickType_t ticks;
} __SDeferredEventData;

class SmState
{
    friend class SmEngine2;
public:
    SmState(const char *name);
    virtual ~SmState() = default;

    virtual bool begin();
    virtual void end();

    virtual void enter();
    virtual void run();
    virtual void exit();
    virtual EEventResult on_event(SEventData event);

protected:
    uint8_t m_id;

    bool switch_state(uint8_t new_state);
    bool pop_state();
    bool push_state(uint8_t new_state);
    uint64_t get_micros();

    /**
     * Returns true, when timeout takes place
     * @param timeout timeout in microseconds
     * @param generate_event true if state machine timeout event should be generated
     * @return true if timeout, false otherwise
     */
    bool timeout_event(uint64_t timeout, bool generate_event = false);
    void reset_timeout();
    const char *get_name() { return m_name; }

    template <typename T>
    T &get_engine() { return *static_cast<T*>(m_engine); }

    virtual uint8_t get_id() { return m_id; }

private:
    const char * m_name=nullptr;
    SmEngine2 *m_engine = nullptr;

    void set_engine( SmEngine2 &engine );
    void set_id(uint8_t id) { m_id = id; }
};

