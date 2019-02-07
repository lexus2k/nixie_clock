#include "nixie_tube_animated.h"
#include "nixie_display.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define CHARS_PER_TUBE   (3)

#define DEBUG

static uint64_t micros()
{
    return (uint64_t)esp_timer_get_time();
}

static const char *get_tube_str(std::string &str, int index)
{
    if ( index * CHARS_PER_TUBE < str.size() )
    {
        return &str.c_str()[ index * CHARS_PER_TUBE ];
    }
    return nullptr;
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
    switch (m_mode)
    {
        case NixieDisplay::Mode::WRAP:
            do_wrap();
            break;
        case NixieDisplay::Mode::ORDERED_WRAP:
        case NixieDisplay::Mode::ORDERED_WRAP_ONCE:
            do_ordered_wrap();
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

void NixieDisplay::set(const char *p)
{
#if 0
    static char b[20]{};
    if (strncmp(b, p, 4))
    {
        strcpy(b,p);
        fprintf(stderr, "\r%s ", p);
    }
#endif
    m_new_value = p;
    switch (m_mode)
    {
        case NixieDisplay::Mode::ORDERED_WRAP_ONCE:
        case NixieDisplay::Mode::ORDERED_WRAP:
            set_effect( NixieTubeAnimated::Effect::IMMEDIATE );
            if ( m_mode_step >= 0)
            {
                m_mode_steps_repeat = true;
            }
            else
            {
                m_mode_step = 0;
            }
            break;
        case NixieDisplay::Mode::WRAP:
        case NixieDisplay::Mode::NORMAL:
        default:
            m_value = m_new_value;
            __set();
            break;
    };
    m_last_us = micros();
}

void NixieDisplay::__set()
{
    int position = -m_position;
    printf("\r");
    for(int i=0; get_by_index(i) != nullptr; i++)
    {
        NixieTubeAnimated *tube = get_by_index(i);
        if ( position < 0 )
        {
            printf(" ");
            tube->set( "   " );
            position++;
        }
        else
        {
            const char *p = get_tube_str( m_value, position );
            tube->set( p ? p : "   " );
            position++;
            // TODO: Add printf of current digit
        }
    }
    printf("  \r");
    fflush(stdout);
}

void NixieDisplay::set_effect(NixieTubeAnimated::Effect effect)
{
    for (int i=0; (get_by_index(i) != nullptr); i++ )
    {
        get_by_index(i)->set_effect( effect );
    }
}

void NixieDisplay::set_mode(NixieDisplay::Mode mode)
{
    m_mode = mode;
    m_last_us = micros();
    m_position = 0;
    m_mode_step = -1;
    m_mode_steps_repeat = false;
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
    if (us - m_last_us >= 700000)
    {
        if ( get_tube_str( m_value, digit_count() ) == nullptr )
        {
            // No need to move digits if all of them can fit to display
            m_position = 0;
        }
        else if ( m_position < 0 && get_tube_str( m_value, -m_position ) == nullptr )
        {
            // If all string went left completely, start from the right side
            m_position = digit_count();
        }
        else
        {
            m_position--;
        }
        m_last_us = us;
        set( m_value.c_str() );
    }
}

void NixieDisplay::do_ordered_wrap()
{
    uint64_t us = micros();
    if (us - m_last_us >= 200000 && m_mode_step >= 0)
    {
        NixieTubeAnimated* tube = get_by_index( m_mode_step );
        if (tube)
        {
            tube->set_effect( NixieTubeAnimated::Effect::SCROLL );
            const char *p = get_tube_str( m_new_value, m_mode_step );
            tube->set( p ? p : "   " );
            m_mode_step++;
        }
        else
        {
            if ( m_mode == NixieDisplay::Mode::ORDERED_WRAP_ONCE )
            {
                m_mode = NixieDisplay::Mode::NORMAL;
                m_value = m_new_value;
            }
            else if ( m_mode_steps_repeat )
            {
                m_mode_step = 0;
            }
            else
            {
                m_value = m_new_value;
                m_mode_step = -1;
            }
        }
        m_last_us = us;
    }
}
