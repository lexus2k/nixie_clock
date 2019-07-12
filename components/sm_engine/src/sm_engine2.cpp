#include "sm_engine2.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <stdio.h>

#define MAX_APP_QUEUE_SIZE   10

static const char* TAG = "SME";

SmEngine2::SmEngine2(int max_queue_size)
    : m_queue( xQueueCreate( max_queue_size, sizeof( __SDeferredEventData ) ) )
{
}

SmEngine2::~SmEngine2()
{
    for (auto i: m_states)
    {
        if (i.auto_allocated)
        {
            delete i.state;
        }
    }
    m_states.clear();
}

bool SmEngine2::send_event(SEventData event)
{
    return send_delayed_event(event, 0);
}

bool SmEngine2::send_delayed_event(SEventData event, uint32_t ms)
{
    __SDeferredEventData ev = {
        .event = event,
        .ticks = xTaskGetTickCount() + ms / portTICK_PERIOD_MS
    };
    if ( xQueueSend( m_queue, (void*)&ev, (TickType_t) 10 ) != pdTRUE )
    {
        ESP_LOGE(TAG, "Failed to put message: %d", event.event);
        return false;
    }
    return true;
}

void SmEngine2::loop(uint32_t event_wait_timeout_ms)
{
    while ( update( event_wait_timeout_ms ) )
    {
    }
}

EEventResult SmEngine2::process_event(SEventData &event)
{
    EEventResult result = on_event( event );
    if ( result != EEventResult::PROCESSED_AND_HOOKED )
    {
        EEventResult state_result = m_active->on_event( event );
        if ( state_result != EEventResult::NOT_PROCESSED )
        {
            result = state_result;
        }
    }
    if ( result == EEventResult::NOT_PROCESSED )
    {
        ESP_LOGW(TAG, "Event is not processed: %i, %X",
                 event.event, event.arg );
    }
    return result;
}

bool SmEngine2::update(uint32_t event_wait_timeout_ms)
{
    on_update();
    if (!m_active)
    {
        ESP_LOGE(TAG, "Initial state is not specified!");
        return false;
    }
    TickType_t ticks = xTaskGetTickCount();
    for(;;)
    {
        __SDeferredEventData event;
        if ( xQueueReceive( m_queue, &event, event_wait_timeout_ms / portTICK_PERIOD_MS ) == pdTRUE )
        {
            if ( ticks >= event.ticks )
            {
                process_event( event.event );
            }
            else
            {
                m_events.push_back( event );
            }
            break;
        }
        else
        {
            break;
        }
    }
    auto it = m_events.begin();
    while ( it != m_events.end() )
    {
        if ( ticks >= it->ticks )
        {
            process_event( it->event );
            it = m_events.erase( it );
        }
        else
        {
            it++;
        }
    }
    m_active->run();

    return !m_stopped;
}

EEventResult SmEngine2::on_event(SEventData event)
{
    return EEventResult::NOT_PROCESSED;
}

void SmEngine2::on_update()
{
}

void SmEngine2::add_state(SmState &state)
{
    register_state( state, false );
}

void SmEngine2::register_state(SmState &state, bool auto_allocated)
{
    SmStateInfo info =
    {
        .state = &state,
        .auto_allocated = auto_allocated
    };
    state.set_engine( *this );
    m_states.push_back( info );
}

bool SmEngine2::begin()
{
    bool result = on_begin();
    if ( result )
    {
        for ( auto i: m_states )
        {
            result = i.state->begin();
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
    for (auto i: m_states)
    {
        i.state->end();
    };
    on_end();
}

void SmEngine2::on_end()
{
}

bool SmEngine2::switch_state(uint8_t id)
{
    for (auto i: m_states)
    {
        if ( i.state->get_id() == id )
        {
            if ( m_active )
            {
                if ( m_active->get_id() == id )
                {
                    break;
                }
                m_active->exit();
            }
            ESP_LOGI(TAG, "Switching to state %s", i.state->get_name());
            m_active = i.state;
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

uint8_t SmEngine2::get_state_id()
{
    return m_active ? m_active->get_id() : 0;
}
