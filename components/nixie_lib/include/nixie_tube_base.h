#pragma once

#include "pin_group_controller.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieTubeBase
{
public:
    NixieTubeBase();
    ~NixieTubeBase() = default;

    void set_anod(int anod_offset, PinGroupController* anods);
    void set_cathodes(int cathodes_offset, PinGroupController* cathodes);

    virtual void begin();
    virtual void end();
    virtual void update();

    void set(int digit);
    void set(char ch);
    virtual void set(const char* str) = 0;

    void update_value(int digit);
    void on();
    void off();
    void set_brightness(uint8_t brightness);

protected:
    // platform specific
    void update_brightness();
    void disable_cathode(int number);
    void enable_cathode(int number);

    void disable_cathodes();
    void enable_cathodes();

private:
    int m_cathodes_offset = -1;
    PinGroupController* m_cathodes = nullptr;
    uint16_t m_enabled_cathodes = 0;
    int m_anod_offset = -1;
    PinGroupController* m_anods = nullptr;
//    int m_pin = -1;
    bool m_enabled = false;
//    int m_value = 0;
//    bool m_pwmMode = false;
    uint8_t m_brightness = 0;
    uint8_t m_target_brightness = 0;
};


