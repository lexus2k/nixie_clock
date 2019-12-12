#include "nixie_tube_animated.h"
#include "nixie_display.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cstdlib>

#define CHARS_PER_TUBE   (3)
// 1. update
// 2. set
// 3. set_mode

#define DEBUG

static uint64_t micros()
{
    return (uint64_t)esp_timer_get_time();
}

NixieTubeAnimated& NixieDisplay::operator [](int index)
{
    NixieTubeAnimated* tube = get_by_index(index);
    if (tube == nullptr)
    {
        tube = &m_fake_tube;
    }
    return *tube;
}

int NixieDisplay::digit_count()
{
    int count = 0;
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        count++;
    }
    return count;
}

void NixieDisplay::do_for_each(const std::function<void(NixieTubeAnimated &tube)> &func)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        func( *get_by_index(i) );
    }
}

void NixieDisplay::begin()
{
    m_value.resize( digit_count(), "   " );
    m_new_value.resize( 0 );
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->begin();
    }
    m_last_us = micros();
}

void NixieDisplay::end()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->end();
    }
}

void NixieDisplay::update()
{
    if (m_value.size() == 0)
    {
        return;
    }
    switch (m_mode)
    {
        case NixieDisplay::Mode::WRAP:
            do_wrap();
            break;
        case NixieDisplay::Mode::ORDERED_WRAP_FROM_LEFT:
            do_ordered_wrap();
            break;
        case NixieDisplay::Mode::ORDERED_WRAP_FROM_RIGHT:
            do_ordered_wrap_right_to_left();
            break;
        case NixieDisplay::Mode::SWIPE_RIGHT:
            do_swipe_right();
            break;
        case NixieDisplay::Mode::SWIPE_LEFT:
            do_swipe_left();
            break;
        case NixieDisplay::Mode::NORMAL:
        default:
            m_last_us = micros();
            break;
    };
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->update();
    }
}

void NixieDisplay::set(const std::string &p)
{
    m_new_value.resize((p.size() + CHARS_PER_TUBE - 1) / CHARS_PER_TUBE, "    " );
    for ( int i=0; i < m_new_value.size(); i++ )
    {
        if ( i * CHARS_PER_TUBE < p.size() )
        {
            m_new_value[i] = p.substr(i * CHARS_PER_TUBE, 3);
        }
        else
        {
            m_new_value[i] = std::string("   ");
        }
        m_new_value[i].resize( CHARS_PER_TUBE, ' ' );
    }
    if ( m_mode_step < 0 )
    {
        switch (m_mode)
        {
            case NixieDisplay::Mode::ORDERED_WRAP_FROM_LEFT:
                set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
                m_mode_step = 0;
                break;
            case NixieDisplay::Mode::ORDERED_WRAP_FROM_RIGHT:
                set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
                m_mode_step = digit_count() - 1;
                break;
            case NixieDisplay::Mode::SWIPE_RIGHT:
            case NixieDisplay::Mode::SWIPE_LEFT:
                set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
                m_mode_step = 0;
                break;
            case NixieDisplay::Mode::WRAP:
                set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
                m_mode_steps_repeat = true;
                m_mode_step = 0;
                break;
            case NixieDisplay::Mode::NORMAL:
            default:
                apply_new_value();
                __set();
                break;
        };
    }
    m_last_us = micros();
}

void NixieDisplay::apply_new_value()
{
    for (int i=0; i< m_value.size(); i++ )
    {
        if ( i < m_new_value.size() )
        {
            m_value[i] = m_new_value[i];
        }
        else
        {
            m_value[i] = std::string("   ");
        }
    }
}

void NixieDisplay::__set()
{
    for(int i=0; get_by_index(i) != nullptr; i++)
    {
        NixieTubeAnimated *tube = get_by_index(i);
        tube->set( m_value[ i ].c_str() );
    }
}

void NixieDisplay::print()
{
    std::string content = "";
    for(int i=0; get_by_index(i) != nullptr; i++)
    {
        NixieTubeAnimated *tube = get_by_index(i);
        content += tube->get_content();
    }
    fprintf( stdout, "\rclock: %s", content.c_str() );
    fflush(stdout);
}

void NixieDisplay::set_effect(NixieTubeAnimated::Effect effect)
{
    for (int i=0; (get_by_index(i) != nullptr); i++ )
    {
        get_by_index(i)->set_effect( effect );
    }
}

void NixieDisplay::set_mode(NixieDisplay::Mode mode, NixieDisplay::Mode next_mode)
{
    m_mode = mode;
    m_next_mode = next_mode;
    m_last_us = micros();
    m_mode_step = -1;
    m_mode_steps_repeat = false;
}

void NixieDisplay::set_random_mode()
{
    int i = std::rand() % 4;
    NixieDisplay::Mode mode;
    switch (i)
    {
        case 0: mode = NixieDisplay::Mode::ORDERED_WRAP_FROM_LEFT; break;
        case 1: mode = NixieDisplay::Mode::ORDERED_WRAP_FROM_RIGHT; break;
        case 2: mode = NixieDisplay::Mode::SWIPE_LEFT; break;
        case 3: mode = NixieDisplay::Mode::SWIPE_RIGHT; break;
        default: mode = NixieDisplay::Mode::SWIPE_RIGHT; break;
    }
    set_mode( mode );
}

void NixieDisplay::on()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->on();
    }
}

void NixieDisplay::off()
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->off();
    }
}

void NixieDisplay::off(uint32_t delay_us)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->off( delay_us );
    }
}

void NixieDisplay::set_brightness(uint8_t brightness)
{
    for (int i=0; get_by_index(i) != nullptr; i++ )
    {
        get_by_index(i)->set_brightness(brightness);
    }
}

void NixieDisplay::do_wrap()
{
    uint64_t us = micros();
    if ((us - m_last_us >= 700000) && m_mode_step >=0)
    {
        for (int i=0; i<m_value.size(); i++)
        {
            int index = i + m_mode_step - m_value.size();
            if (index >= 0 && index < m_new_value.size())
            {
                m_value[i] = m_new_value[index];
            }
            else
            {
                m_value[i] = std::string( "   " );
            }
        }
        m_mode_step++;
        if ( m_mode_step >= m_new_value.size() + m_value.size() )
        {
            m_mode_step = 0;
            m_mode = m_next_mode;
        }
        __set();
        m_last_us = us;
    }
}

void NixieDisplay::do_ordered_wrap()
{
    uint64_t us = micros();
    if (us - m_last_us >= 100000 && m_mode_step >= 0)
    {
        NixieTubeAnimated* tube = get_by_index( m_mode_step );
        if (tube)
        {
            tube->set_effect( NixieTubeAnimated::Effect::SCROLL );
            if ( m_mode_step < m_new_value.size() )
            {
                m_value[ m_mode_step ] = m_new_value[ m_mode_step ];
            }
            else
            {
                m_value[ m_mode_step ] = std::string("   ");
            }
            tube->set( m_value[ m_mode_step ].c_str() );
            m_mode_step++;
        }
        else
        {
            m_mode = m_next_mode;
            m_mode_step = -1;
        }
        m_last_us = us;
    }
}

void NixieDisplay::do_ordered_wrap_right_to_left()
{
    uint64_t us = micros();
    if ((us - m_last_us >= 100000) && m_mode_step >= 0)
    {
        NixieTubeAnimated* tube = get_by_index( m_mode_step );
        if (tube)
        {
            if ( m_mode_step < m_new_value.size() )
            {
                m_value[ m_mode_step ] = m_new_value[ m_mode_step ];
            }
            else
            {
                m_value[ m_mode_step ] = std::string( "   " );
            }
            tube->set_effect( NixieTubeAnimated::Effect::SCROLL );
            tube->set( m_value[ m_mode_step ].c_str() );
            m_mode_step--;
        }
        if ( m_mode_step < 0 )
        {
            m_mode = m_next_mode;
            if ( m_mode == NixieDisplay::Mode::ORDERED_WRAP_FROM_RIGHT )
            {
                m_mode_step = digit_count() - 1;
            }
        }
        m_last_us = us;
    }
}

void NixieDisplay::do_swipe_right()
{
    uint64_t us = micros();
    if ((us - m_last_us >= 30000) && m_mode_step >= 0)
    {
        int index = m_new_value.size() - 1 - m_mode_step + m_value.size() * 2;
        m_value.pop_back();
        if ( index >=0 && index < m_new_value.size() )
        {
            m_value.emplace( m_value.begin(), m_new_value[ index ] );
        }
        else
        {
            m_value.emplace( m_value.begin(), std::string("   ") );
        }
        __set();
        m_mode_step++;
        if ( index == 0 )
        {
            m_mode_step = -1;
            m_mode = m_next_mode;
        }
        m_last_us = us;
    }
}

void NixieDisplay::do_swipe_left()
{
    uint64_t us = micros();
    if (us - m_last_us >= 30000 && m_mode_step >= 0)
    {
        int index = -m_value.size() * 2 + m_mode_step;
        m_value.erase( m_value.begin() );
        if ( index >= 0 && index < m_new_value.size() )
        {
            m_value.emplace_back( m_new_value[ index ] );
        }
        else
        {
            m_value.emplace_back( std::string("   ") );
        }
        __set();
        m_mode_step++;
        if ( index == m_new_value.size() - 1 )
        {
            m_mode_step = -1;
            m_mode = m_next_mode;
        }
        m_last_us = us;
    }
}
