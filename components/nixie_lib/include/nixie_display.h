#pragma once

#include "nixie_tube_fake.h"
#include "fake_group_controller.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <string.h>
#include <functional>

class NixieDisplay
{
public:
    enum class Effect: uint8_t
    {
        IMMEDIATE = NixieTubeAnimated::Effect::IMMEDIATE,
        SCROLL = NixieTubeAnimated::Effect::SCROLL,
        OVERLAP = NixieTubeAnimated::Effect::OVERLAP,
        LAST,
    };

    enum class Mode: uint8_t
    {
        NORMAL,
        WRAP,
    };


    NixieDisplay() = default;
    ~NixieDisplay() = default;

    NixieTubeAnimated& operator [](int index);

    void set(const char *p);
    void set_effect( NixieDisplay::Effect effect );
    void set_mode( NixieDisplay::Mode mode );

    void on();
    void off();
    void off(uint32_t delay_us);
    void set_brightness(uint8_t brightness);
    int digit_count();

    virtual void begin();
    virtual void end();
    virtual void update();

protected:
    virtual NixieTubeAnimated* get_by_index(int index) = 0;
    void do_for_each(const std::function<void(NixieTubeAnimated &tube)> &func);

private:
    NixieTubeFake m_fake_tube;
    uint64_t m_last_us = 0;
    std::string m_value;
    NixieDisplay::Mode m_mode = NixieDisplay::Mode::NORMAL;
    int m_position = 0;

    void __set(const char *p);
    void do_wrap();
};

