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

void SmState::switch_state(uint8_t new_state)
{
    if (m_engine)
    {
        m_engine->switch_state( new_state );
    }
}

void SmState::set_engine( SmEngine2 &engine )
{
    m_engine = &engine;
}

