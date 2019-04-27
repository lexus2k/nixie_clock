#include "sm_engine2.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <stdio.h>

#define MAX_APP_QUEUE_SIZE   10

static const char* TAG = "SME";

SmEngine2::SmEngine2(int max_queue_size)
    : m_queue( xQueueCreate( max_queue_size, sizeof( SEventData ) ) )
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

bool SmEngine2::send_delayed_event(SEventData event, uint32_t ms)
{
    if ( m_timer_event.event == 0 )
    {
        m_timer_event = event;
        m_timer_ticks = xTaskGetTickCount() + ms / portTICK_PERIOD_MS;
        return true;
    }
    return false;
}

void SmEngine2::loop(uint32_t event_wait_timeout_ms)
{
    while ( update( event_wait_timeout_ms ) )
    {
    }
}

bool SmEngine2::update(uint32_t event_wait_timeout_ms)
{
    on_update();
    if (!m_active)
    {
        ESP_LOGE(TAG, "Initial state is not specified!");
        return false;
    }
    for(;;)
    {
        SEventData event;
        if ( xQueueReceive( m_queue, &event, event_wait_timeout_ms / portTICK_PERIOD_MS ) == pdTRUE )
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
            break;
        }
        else
        {
            break;
        }
    }
    m_active->run();
    if ( m_timer_event.event != 0 )
    {
        TickType_t ticks = xTaskGetTickCount();
        if ( ticks >= m_timer_ticks )
        {
            send_event( m_timer_event );
            m_timer_event = SEventData{};
        }
    }

    return !m_stopped;
}

bool SmEngine2::on_event(SEventData event)
{
    return false;
}

void SmEngine2::on_update()
{
}

void SmEngine2::add_state(SmState &state)
{
    state.set_engine( *this );
    state.m_next = m_first;
    m_first = &state;
}

bool SmEngine2::begin()
{
    bool result = on_begin();
    if ( result )
    {
        for(SmState *p = m_first; p != nullptr; p = p->m_next)
        {
            result = p->begin();
            if ( !result )
            {
                break;
            }
        }
    }
    m_stopped = false;
    return result;
}

bool SmEngine2::on_begin()
{
    return true;
}

void SmEngine2::end()
{
    if (m_active)
    {
        m_active->exit();
    }
    for(SmState *p = m_first; p != nullptr; p = p->m_next)
    {
        p->end();
    };
    on_end();
}

void SmEngine2::on_end()
{
}

bool SmEngine2::switch_state(uint8_t id)
{
    for(SmState *p = m_first; p != nullptr; p = p->m_next)
    {
        if ( p->get_id() == id )
        {
            if ( m_active )
            {
                if ( m_active->get_id() == id )
                {
                    break;
                }
                m_active->exit();
            }
            ESP_LOGI(TAG, "Switching to state %s", p->get_name());
            m_active = p;
            m_active->enter();
            return true;
        }
    };
    return false;
}

bool SmEngine2::push_state(uint8_t new_state)
{
    m_stack.push(m_active);
    bool result = switch_state(new_state);
    if (!result)
    {
        m_stack.pop();
    }
    return result;
}

bool SmEngine2::pop_state()
{
    bool result = false;
    if (!m_stack.empty())
    {
        auto state = m_stack.top();
        m_stack.pop();
        result = switch_state(state->get_id());
        if (!result)
        {
            m_stack.push(state);
        }
    }
    return result;
}

