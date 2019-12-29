#include "led_controller.h"
#include "platform/system.h"

void LedController::begin()
{
    m_hw.setup({0b1100000, 0b1100001}, {
        { {0, 0}, {0, 1}, {0, 2} },
        { {0, 3}, {0, 4}, {0, 5} },
        { {0, 6}, {0, 7}, {0, 8} },
        { {1, 0}, {1, 1}, {1, 2} },
        { {1, 3}, {1, 4}, {1, 5} },
        { {1, 6}, {1, 7}, {1, 8} },
        } );

    // init led controllers
    m_hw.set_min_pwm(3, 3, 3);
    m_hw.set_max_pwm(232,175,112);

    // Leds must be started before RTC chip, because TLC59116
    // uses ALLCALADR=1101000, the same as DS3232 (1101000)
    m_hw.begin();
}

void LedController::update()
{
    m_hw.update();
    if ( m_status != LedStatus::NORMAL )
    {
        uint32_t delta = static_cast<uint32_t>(millis() - m_start_time_ms);
        if ( m_status == LedStatus::STA_DISCONNECTED && delta >= 60000 )
        {
            set_status( LedStatus::NORMAL );
        }
        if ( m_status == LedStatus::UPGRADE_FAILED && delta >= 60000 )
        {
            set_status( LedStatus::NORMAL );
        }
        if ( m_status == LedStatus::BOOTING && delta >= 60000 )
        {
            set_status( LedStatus::NORMAL );
        }
    }
}

void LedController::end()
{
    m_hw.end();
}

void LedController::set_color( LedsMode mode, uint32_t color )
{
    set_color( color );
    set_mode( mode );
}

void LedController::set_color( int index, uint32_t color )
{
    m_hw.set_color( index, color );
}

void LedController::set_brightness(uint8_t br)
{
    m_hw.set_brightness( br );
}

void LedController::set_color( uint32_t color )
{
    m_color = color;
    if ( m_status == LedStatus::NORMAL )
    {
        m_hw.set_color( color );
    }
}

void LedController::set_mode( LedsMode mode )
{
    m_mode = mode;
    if ( m_status == LedStatus::NORMAL )
    {
        m_hw.set_mode( mode );
    }
}

void LedController::on()
{
    m_hw.enable();
}

void LedController::off()
{
    m_hw.disable();
}

void LedController::set_status( LedStatus status )
{
    if ( status != m_status && status != LedStatus::NORMAL )
    {
        m_start_time_ms = millis();
    }
    m_status = status;
    switch ( m_status )
    {
        case LedStatus::NORMAL:
            m_hw.set_color( m_color );
            m_hw.set_mode( m_mode );
            m_hw.disable_blink();
            break;
        case LedStatus::BOOTING:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color( 0x00004000 );
            m_hw.set_color( 2, 0x00000080 );
            m_hw.set_color( 3, 0x00000080 );
            break;
        case LedStatus::BOOT_FAILED:
            m_hw.set_color( 0x00C04040 );
            m_hw.enable_blink();
            break;
        case LedStatus::AP_STARTED:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color(2, 0x00000080 );
            m_hw.set_color(3, 0x00000080 );
            break;
        case LedStatus::AP_CLIENT_CONNECTED:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color(2, 0x00404000);
            m_hw.set_color(3, 0x00404000);
            break;
        case LedStatus::STA_DISCONNECTED:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color( 2, 0x00000080 );
            m_hw.set_color( 3, 0x00000080 );
            break;
        case LedStatus::UPGRADE_IN_PROGRESS:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color( 0x00000030 );
            m_hw.enable_blink();
            break;
        case LedStatus::UPGRADE_SUCCESSFUL:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color( 0x00008000 );
            break;
        case LedStatus::UPGRADE_FAILED:
            m_hw.set_mode( LedsMode::NORMAL );
            m_hw.set_color( 0x00800000 );
            break;
        default: break;
    }
}
