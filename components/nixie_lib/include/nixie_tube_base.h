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
    virtual ~NixieTubeBase() = default;

    void set_anod(int anod_offset, PinGroupController* anods);
    void set_cathodes(int cathodes_offset, PinGroupController* cathodes);

    virtual void begin();
    virtual void end();
    virtual void update();

    void on();
    void off();
    void set_brightness(uint8_t brightness);
    uint8_t get_brightness();

protected:
    // platform specific
    void disable_cathode(int number);
    void enable_cathode(int number);
    void enable_anod();
    void disable_anod();
    void fake_off();
    uint8_t get_actual_brightness();
    int get_enabled_cathode(int prev = -1);
    void set_user_brightness_fraction(uint8_t fraction);
    uint8_t get_user_brightness_fraction();

private:
    /** first tube cathode number in PinGroupController. Controller controls multiple tubes */
    int m_cathodes_offset = -1;
    /** user brightness fraction in percents. By default use 100% of brightness set by user */
    uint8_t m_user_brightness_fraction = 100;
    PinGroupController* m_cathodes = nullptr;
    uint16_t m_enabled_cathodes = 0;
    bool m_enabled_anod = false;
    int m_anod_offset = -1;
    PinGroupController* m_anods = nullptr;
    bool m_enabled = false;
    uint8_t m_brightness = 0;
    uint8_t m_fake_brightness = 255;

    void disable_cathodes();
    void enable_cathodes();
    void disable_anods();
    void enable_anods();
};


