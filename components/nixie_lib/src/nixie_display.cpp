#include "nixie_tube_animated.h"
#include "nixie_display.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
#ifdef DEBUG
    static char b[20]{};
    if (strncmp(b, p, 4))
    {
        strcpy(b,p);
        fprintf(stderr, "%s\n", p);
    }
#endif
    m_value = p;
    __set(p);
}

void NixieDisplay::__set(const char *p)
{
    int position = 0;
    if (m_position < 0)
    {
        position = m_position;
        while (position < 0)
        {
            NixieTubeAnimated *tube = get_by_index( position - m_position );
            if (tube != nullptr)
            {
                tube->set("   ");
            }
            position++;
        }
    }
    if (m_position > 0)
    {
        const int len = strlen(p);
        p += (m_position >= len ? len: m_position);
    }
    while ((get_by_index(position) != nullptr) && (*p != '\0'))
    {
        const char *next = get_by_index(position)->set( p );
        if ( next != p ) p = next;
        position++;
    }
    while (get_by_index(position) != nullptr)
    {
        get_by_index(position)->set("   "); // disable not needed tubes
        position++;
    }
}

void NixieDisplay::set_effect(NixieDisplay::Effect effect)
{
    for (int i=0; (get_by_index(i) != nullptr); i++ )
    {
        get_by_index(i)->set_effect( static_cast<NixieTubeAnimated::Effect>(effect) );
    }
}

void NixieDisplay::set_mode(NixieDisplay::Mode mode)
{
    m_mode = mode;
    m_position = 0;
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
        if ( m_value.size() - m_position <= digit_count() )
        {
            m_position = 0;
        }
        else
        {
            m_position++;
        }
        m_last_us = us;
        set( m_value.c_str() );
    }
}