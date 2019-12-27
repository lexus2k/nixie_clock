#pragma once

#include "hardware/rgb_leds.h"
#include <stdint.h>

enum class LedStatus: uint8_t
{
    NORMAL,
    BOOTING,
    BOOT_FAILED,
    AP_STARTED,
    AP_CLIENT_CONNECTED,
    STA_DISCONNECTED,
    UPGRADE_IN_PROGRESS,
    UPGRADE_SUCCESSFUL,
    UPGRADE_FAILED,
};

class LedController
{
public:
    LedController(Tlc59116Leds &hw): m_hw(hw) {}
    ~LedController() = default;
    void begin();
    void update();
    void end();

    void on();
    void off();
    void set_color( LedsMode mode, uint32_t color );
    void set_color( uint32_t color );
    void set_color( int index, uint32_t color );
    void set_mode( LedsMode mode );
    void set_brightness(uint8_t br);

    void set_status( LedStatus status );
private:
    Tlc59116Leds &m_hw;
    LedsMode m_mode = LedsMode::NORMAL;
    uint32_t m_color = 0x00000000;
    uint32_t m_start_time_ms = 0;
    LedStatus m_status = LedStatus::NORMAL;
};
