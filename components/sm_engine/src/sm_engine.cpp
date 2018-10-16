/*
    Copyright (C) 2016-2018 Alexey Dynda

    This file is part of Nixie Library.

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

#include "sm_engine.h"
#include <stdio.h>

#define MAX_APP_QUEUE_SIZE   10

SmEngine::SmEngine(const state_info_t *states)
    : m_states( states )
    , m_queue( xQueueCreate( MAX_APP_QUEUE_SIZE, sizeof( uint32_t ) ) )
{
}

bool SmEngine::send_event(uint8_t id, uint8_t param)
{
    uint32_t msg = (id << 8) | (param);
    if ( xQueueSend( m_queue, ( void * ) &msg, ( TickType_t ) 10 ) != pdTRUE )
    {
        return false;
    }
    return true;
}

void SmEngine::push_state(uint8_t id)
{
    if ( m_active_state )
    {
        m_pop_state = m_active_state->id;
    }
    switch_state(id);
}

void SmEngine::pop_state()
{
    switch_state(m_pop_state);
}

void SmEngine::switch_state(uint8_t id)
{
    uint8_t i=0;
    for(;;)
    {
        uint8_t next_id = m_states[i].id;
        if (next_id == STATE_ID_INVALID)
        {
            break;
        }
        if (next_id == id)
        {
            if ( m_active_state )
            {
                m_active_state->exit_cb();
            }
            fprintf(stderr, "[SM] switching to %s\n", m_states[i].name);
            m_active_state = &m_states[i];
            if (m_active_state->enter_cb)
            {
                m_active_state->enter_cb();
            }
            break;
        }
        i++;
    };
}


void SmEngine::loop()
{
    for(;;)
    {
        update();
    }
}


void SmEngine::update()
{
    if (!m_active_state)
    {
        return;
    }
    uint32_t msg;
    if ( xQueueReceive( m_queue, &msg, 0 ) == pdTRUE )
    {
        m_active_state->event_cb(msg >> 8, msg & 0x00FF);
    }
    m_active_state->state_cb();
}


