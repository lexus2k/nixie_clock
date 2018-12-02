#include "sm_engine2.h"
#include "esp_log.h"
#include <stdio.h>

#define MAX_APP_QUEUE_SIZE   10

static const char* TAG = "SME";

SmEngine2::SmEngine2()
    : m_queue( xQueueCreate( MAX_APP_QUEUE_SIZE, sizeof( SEventData ) ) )
{
}

bool SmEngine2::send_event(SEventData event)
{
    if ( xQueueSend( m_queue, (void*)&event, (TickType_t) 10 ) != pdTRUE )
    {
        return false;
    }
    return true;
}

void SmEngine2::switch_state(uint8_t id)
{
    for(SmState *p = m_first; p != nullptr; p = p->m_next)
    {
        if ( p->get_id() == id )
        {
            if ( m_active )
            {
                m_active->exit();
            }
            ESP_LOGI(TAG, "Switching to state %s", p->get_name());
            m_active = p;
            m_active->enter();
            break;
        }
    };
}

void SmEngine2::loop()
{
    for(;;)
    {
        update();
    }
}

void SmEngine2::update()
{
    if (!m_active)
    {
        return;
    }
    SEventData event;
    if ( xQueueReceive( m_queue, &event, 0 ) == pdTRUE )
    {
        bool result = on_event( event );
        if ( !result )
        {
            result = m_active->on_event( event );
        }
        if ( !result )
        {
            ESP_LOGW(TAG, "Event is not processed: %i, %X",
                     event.event, event.arg );
        }
    }
    m_active->run();
}

bool SmEngine2::on_event(SEventData event)
{
    return false;
}

void SmEngine2::add_state(SmState &state)
{
    state.set_engine( *this );
    state.m_next = m_first;
    m_first = &state;
}

bool SmEngine2::begin()
{
    bool result = true;
    for(SmState *p = m_first; p != nullptr; p = p->m_next)
    {
        result = p->begin();
        if ( !result )
        {
            break;
        }
    };
    return result;
}

void SmEngine2::end()
{
    for(SmState *p = m_first; p != nullptr; p = p->m_next)
    {
        p->end();
    };
}
