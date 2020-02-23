#include "sm_engine2.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <stdio.h>
#include <chrono>

#define MAX_APP_QUEUE_SIZE   10

enum
{
    EV_TYPE_APP,
    EV_TYPE_INTERNAL,
};

enum
{
    INT_EVT_SWITCH_STATE,
    INT_EVT_PUSH_STATE,
    INT_EVT_POP_STATE,
};

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
    return do_put_event(EV_TYPE_APP, event, 0);
}

bool SmEngine2::send_delayed_event(SEventData event, uint32_t ms)
{
    return do_put_event(EV_TYPE_APP, event, ms);
}

bool SmEngine2::do_put_event(uint8_t type, SEventData event, uint32_t ms)
{
    __SDeferredEventData ev = {
        .event_type = type,
        .event = event,
        .micros = ms * 1000
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

EEventResult SmEngine2::process_app_event(SEventData &event)
{
    EEventResult result = on_event( event );
    if ( result != EEventResult::PROCESSED_AND_HOOKED && m_active )
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

EEventResult SmEngine2::process_int_event(SEventData &event)
{
    EEventResult result = EEventResult::NOT_PROCESSED;
    switch ( event.event )
    {
        case INT_EVT_SWITCH_STATE: do_switch_state( event.arg ); break;
        case INT_EVT_PUSH_STATE: do_push_state( event.arg ); break;
        case INT_EVT_POP_STATE: do_pop_state(); break;
        default:
            ESP_LOGW(TAG, "Internal event is not processed: %i, %X", event.event, event.arg );
            break;
    }
    return result;
}

bool SmEngine2::update(uint32_t event_wait_timeout_ms)
{
    on_update();
    for(;;)
    {
        __SDeferredEventData event;
        if ( xQueueReceive( m_queue, &event, event_wait_timeout_ms / portTICK_PERIOD_MS ) != pdTRUE )
        {
            break;
        }
        if ( event.event_type == EV_TYPE_APP )
        {
            m_events.push_back( event );
        }
        else
        {
            m_events.push_front( event );
        }
    }
    uint32_t ts = get_micros();
    uint32_t delta = static_cast<uint32_t>( ts - m_last_update_time_ms );
    m_last_update_time_ms = ts;

    auto it = m_events.begin();
    while ( it != m_events.end() )
    {
        if ( it->micros <= delta )
        {
            switch ( it->event_type )
            {
                case EV_TYPE_APP: process_app_event( it->event ); break;
                case EV_TYPE_INTERNAL: process_int_event( it->event ); break;
                default:
                    ESP_LOGW(TAG, "Unknown event type: 0x%02X", it->event_type );
                    break;
            }
            it = m_events.erase( it );
        }
        else
        {
            it->micros -= delta;
            it++;
        }
    }
    if (!m_active)
    {
        ESP_LOGE(TAG, "Initial state is not specified!");
        return false;
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
    m_last_update_time_ms = get_micros();
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
    return do_put_event( EV_TYPE_INTERNAL, SEventData{INT_EVT_SWITCH_STATE, id}, 0 );
}

bool SmEngine2::do_switch_state(uint8_t id)
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
            m_state_start_ts = get_micros();
            m_active->enter();
            return true;
        }
    };
    return false;
}

bool SmEngine2::push_state(uint8_t new_state)
{
    return do_put_event( EV_TYPE_INTERNAL, SEventData{INT_EVT_PUSH_STATE, new_state}, 0 );
}

bool SmEngine2::do_push_state(uint8_t new_state)
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
    return do_put_event( EV_TYPE_INTERNAL, SEventData{INT_EVT_POP_STATE, 0}, 0 );
}

bool SmEngine2::do_pop_state()
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

uint8_t SmEngine2::get_id()
{
    return m_active ? m_active->get_id() : 0;
}

uint64_t SmEngine2::get_micros()
{
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
}

bool SmEngine2::timeout_event(uint64_t timeout, bool generate_event)
{
    bool event = static_cast<uint64_t>( get_micros() - m_state_start_ts ) >= timeout;
    if ( event && generate_event )
    {
         send_event( { SM_EVENT_TIMEOUT, static_cast<uintptr_t>(timeout) } );
    }
    return event;
}

void SmEngine2::reset_timeout()
{
    m_state_start_ts = get_micros();
}


