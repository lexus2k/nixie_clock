#include "sm_engine.h"
#include "sme_state.h"
#include "sm_engine_logger.h"
#include <chrono>

#define MAX_APP_QUEUE_SIZE   10

static const char* TAG = "SME";

SmEngine::~SmEngine()
{
    for (auto i: m_states)
    {
#if SM_ENGINE_DYNAMIC_ALLOC
        if (i.auto_allocated)
        {
            delete i.state;
        }
#endif
    }
    m_states.clear();
}

bool SmEngine::send_event(SEventData event)
{
    return send_delayed_event(event, 0);
}

bool SmEngine::send_delayed_event(SEventData event, uint32_t ms)
{
    __SDeferredEventData ev =
    {
        .event = event,
        .micros = ms * 1000
    };
#if SM_ENGINE_MULTITHREAD
    std::unique_lock<std::mutex> lock(m_mutex);
#endif
    if ( m_events.size() >= MAX_APP_QUEUE_SIZE )
    {
        ESP_LOGE( TAG, "Failed to put new event: %02X", event.event );
        return false;
    }
    ESP_LOGD( TAG, "New event arrived: %02X", event.event );
    m_events.push_back( ev );
#if SM_ENGINE_MULTITHREAD
    m_cond.notify_one();
#endif
    return true;
}

void SmEngine::loop(uint32_t event_wait_timeout_ms)
{
    set_wait_event_timeout( event_wait_timeout_ms );
    while (!m_stopped)
    {
        update();
    }
}

EEventResult SmEngine::process_app_event(SEventData &event)
{
    ESP_LOGD( TAG, "Processing event: %02X", event.event );
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

void SmEngine::wait_for_next_event()
{
#if SM_ENGINE_MULTITHREAD
    std::unique_lock<std::mutex> lock( m_mutex );
    m_cond.wait_for( lock, std::chrono::milliseconds( m_event_wait_timeout_ms ),
                     [this]()->bool{ return m_events.size() > 0; } );
#endif
}


void SmEngine::update()
{
    on_update();

    wait_for_next_event();

    uint32_t ts = get_micros();
    uint32_t delta = static_cast<uint32_t>( ts - m_last_update_time_ms );
    m_last_update_time_ms = ts;

    auto it = m_events.begin();
    while ( it != m_events.end() )
    {
        if ( it->micros <= delta )
        {
            process_app_event( it->event );
#if SM_ENGINE_MULTITHREAD
            std::unique_lock<std::mutex> lock( m_mutex );
#endif
            it = m_events.erase( it );
        }
        else
        {
            it->micros -= delta;
            it++;
        }
    }
    if (m_active)
        m_active->update();
    else
        ESP_LOGE(TAG, "Initial state is not specified!");
}

EEventResult SmEngine::on_event(SEventData event)
{
    return EEventResult::NOT_PROCESSED;
}

void SmEngine::on_update()
{
}

void SmEngine::add_state(ISmeState &state)
{
    register_state( state, false );
}

void SmEngine::register_state(ISmeState &state, bool auto_allocated)
{
    SmStateInfo info =
    {
        .state = &state,
        .auto_allocated = auto_allocated
    };
    state.set_parent( this );
    m_states.push_back( info );
}

bool SmEngine::begin()
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

bool SmEngine::on_begin()
{
    return true;
}

void SmEngine::end()
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

void SmEngine::on_end()
{
}

ISmeState *SmEngine::getById(uint8_t id)
{
    for (auto i: m_states)
    {
        if ( i.state->get_id() == id )
        {
            return i.state;
        }
    };
    return nullptr;
}

bool SmEngine::switch_state(uint8_t id)
{
    ISmeState * newState = getById( id );
    if ( newState )
    {
        if ( m_active )
        {
            if ( m_active->get_id() == id )
            {
                return false;
            }
            m_active->exit();
        }
        ESP_LOGI(TAG, "Switching to state %s", newState->get_name());
        m_active = newState;

        m_state_start_ts = get_micros();
        m_active->enter();
        force_set_id( id );
        return true;
    }
    return false;
}

bool SmEngine::push_state(uint8_t new_state)
{
    m_stack.push(m_active);
    bool result = switch_state(new_state);
    if (!result)
    {
        m_stack.pop();
        ESP_LOGE(TAG, "Failed to push state: %02X", new_state);
    }
    else
    {
        ESP_LOGI(TAG, "Push state successful: %02X", new_state);
    }
    return result;
}

bool SmEngine::pop_state()
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
    else
    {
        ESP_LOGE(TAG, "Failed to pop state: stack is empty");
    }
    return result;
}

uint64_t SmEngine::get_micros()
{
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
}

bool SmEngine::timeout_event(uint64_t timeout, bool generate_event)
{
    bool event = static_cast<uint64_t>( get_micros() - m_state_start_ts ) >= timeout;
    if ( event && generate_event )
    {
         send_event( { SM_EVENT_TIMEOUT, static_cast<uintptr_t>(timeout) } );
    }
    return event;
}

void SmEngine::reset_timeout()
{
    m_state_start_ts = get_micros();
}


