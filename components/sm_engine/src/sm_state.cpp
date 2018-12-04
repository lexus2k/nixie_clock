#include "sm_state.h"
#include "sm_engine2.h"


SmState::SmState(const char *name)
    :m_name( name )
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

bool SmState::on_event(SEventData event)
{
    return false;
}

bool SmState::switch_state(uint8_t new_state)
{
    return m_engine ? m_engine->switch_state( new_state ) : false;
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

