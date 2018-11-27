#include "nixie_tube_base.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#define BRIGHTNESS_UPDATE_PERIOD_US   20000

NixieTubeBase::NixieTubeBase()
{
}

void NixieTubeBase::begin()
{
    disable_cathodes();
    disable_anods();
}

void NixieTubeBase::end()
{
    disable_cathodes();
    disable_anods();
}

void NixieTubeBase::set_cathodes(int cathodes_offset, PinGroupController* cathodes)
{
    m_cathodes_offset = cathodes_offset;
    m_cathodes = cathodes;
}

void NixieTubeBase::set_anod(int anod_offset, PinGroupController* anods)
{
    m_anod_offset = anod_offset;
    m_anods = anods;
}

void NixieTubeBase::off()
{
    m_enabled = false;
    disable_cathodes();
    disable_anods();
}

void NixieTubeBase::fake_off()
{
    m_fake_brightness = 0;
    set_brightness( m_brightness );
}

void NixieTubeBase::on()
{
    m_enabled = true;
    m_fake_brightness = 255;
    enable_anods();
    enable_cathodes();
}

void NixieTubeBase::set_brightness(uint8_t brightness)
{
    m_brightness = brightness;
    enable_anods();
    enable_cathodes();
}

void NixieTubeBase::update()
{
}

void NixieTubeBase::enable_anod()
{
    m_enabled_anod = true;
    if ( m_anods != nullptr && m_anod_offset >= 0 && m_enabled )
    {
        if ( !m_anods->set( m_anod_offset, get_actual_brightness() ) )
        {
            m_anods->set( m_anod_offset );
        }
    }
}

void NixieTubeBase::disable_anod()
{
    m_enabled_anod = false;
    if ( m_anods != nullptr && m_anod_offset >=0 )
    {
        m_anods->clear( m_anod_offset );
    }
}

void NixieTubeBase::disable_cathode(int number)
{
    m_enabled_cathodes &= ~(1 << number);
    if ( m_cathodes != nullptr && (number >= 0) )
    {
        m_cathodes->clear(m_cathodes_offset + number);
    }
}

void NixieTubeBase::enable_cathode(int number)
{
    m_enabled_cathodes |= (1 << number);
    if (m_enabled && m_cathodes != nullptr && (number >= 0) )
    {
        if ( !m_cathodes->set(m_cathodes_offset + number, get_actual_brightness() ) )
        {
            m_cathodes->set( m_cathodes_offset + number );
        }
    }
}

void NixieTubeBase::disable_anods()
{
    if ( m_anods != nullptr && m_anod_offset >=0 )
    {
        m_anods->clear( m_anod_offset );
    }
}

void NixieTubeBase::enable_anods()
{
    if ( m_anods != nullptr && m_anod_offset >= 0 && m_enabled && m_enabled_anod )
    {
        if ( !m_anods->set( m_anod_offset, get_actual_brightness() ) )
        {
            m_anods->set( m_anod_offset );
        }
    }
}

void NixieTubeBase::disable_cathodes()
{
    if ( m_cathodes != nullptr )
    {
        uint16_t cathodes = m_enabled_cathodes;
        int index = 0;
        while (cathodes)
        {
            if ( cathodes & 0x01 )
            {
                m_cathodes->clear(m_cathodes_offset + index);
            }
            index++;
            cathodes >>= 1;
        }
    }
}

void NixieTubeBase::enable_cathodes()
{
    if ( ( m_cathodes != nullptr ) && (m_enabled) )
    {
        uint16_t cathodes = m_enabled_cathodes;
        int index = 0;
        while (cathodes)
        {
            if ( cathodes & 0x01 )
            {
                if ( !m_cathodes->set(m_cathodes_offset + index, get_actual_brightness()) )
                {
                    m_cathodes->set( m_cathodes_offset + index );
                }
            }
            index++;
            cathodes >>= 1;
        }
    }
}

uint8_t NixieTubeBase::get_actual_brightness()
{
    return m_fake_brightness > m_brightness ? m_brightness : m_fake_brightness;
}
