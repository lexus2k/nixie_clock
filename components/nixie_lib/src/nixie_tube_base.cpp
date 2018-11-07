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
    if ( m_anod_offset >= 0 && m_anods )
    {
        m_anods->clear( m_anod_offset );
    }
    update_brightness();
}

void NixieTubeBase::end()
{
    if ( m_anod_offset >= 0 && m_anods )
    {
        m_anods->clear( m_anod_offset );
    }
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
    update_brightness();
    disable_cathodes();
}

void NixieTubeBase::on()
{
    m_enabled = true;
    enable_cathodes();
    update_brightness();
}

void NixieTubeBase::set_brightness(uint8_t brightness)
{
    m_brightness = brightness;
    update_brightness();
}

void NixieTubeBase::update()
{
}

void NixieTubeBase::update_brightness()
{
    if ( (m_anods == nullptr) || (m_anod_offset < 0) )
    {
        return;
    }
    if ( m_enabled )
    {
        m_anods->set( m_anod_offset, m_brightness );
    }
    else
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
        m_cathodes->set(m_cathodes_offset + number);
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
                m_cathodes->set(m_cathodes_offset + index);
            }
            index++;
            cathodes >>= 1;
        }
    }
}
