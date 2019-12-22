#include "sm_state.h"
#include "sm_engine2.h"


SmState::SmState(const char *name)
    : m_id( SM_STATE_NONE )
    , m_name( name )
{
}

bool SmState::begin()
{
    return true;
}

void SmState::end()
{
}

void SmState::enter()
{
}

void SmState::run()
{
}

void SmState::exit()
{
}

EEventResult SmState::on_event(SEventData event)
{
    return EEventResult::NOT_PROCESSED;
}

bool SmState::switch_state(uint8_t new_state)
{
    return m_engine ? m_engine->switch_state( new_state ) : false;
}

uint64_t SmState::get_micros()
{
    return m_engine ? m_engine->get_micros() : 0;
}

bool SmState::timeout_event(uint64_t timeout, bool generate_event)
{
    return m_engine ? m_engine->timeout_event( timeout, generate_event ) : false;
}

void SmState::reset_timeout()
{
    if (m_engine) m_engine->reset_timeout();
}

bool SmState::push_state(uint8_t new_state)
{
    return m_engine ? m_engine->push_state( new_state ) : false;
}

bool SmState::pop_state()
{
    return m_engine ? m_engine->pop_state() : false;
}

void SmState::set_engine( SmEngine2 &engine )
{
    m_engine = &engine;
}

