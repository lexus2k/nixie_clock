#pragma once

#include "nixie_tube_fake.h"
#include "fake_group_controller.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <functional>

class NixieDisplay
{
public:
    enum class Mode: uint8_t
    {
        NORMAL,            ///< All digits are replaced by new ones immediately
        WRAP,              ///< All digits start to count from 0 to 9 at once, stopping at new digits values
        ORDERED_WRAP_FROM_LEFT,  ///< Digits starts to count from 0 to 9 one by one from left side to the right
        ORDERED_WRAP_FROM_RIGHT, ///< Digits starts to count from 0 to 9 one by one from right side to the left
        SWIPE_LEFT,        ///< All digits are moving to the left, being replaced by new digits from the right
        SWIPE_RIGHT,       ///< All digits are moving to the right, being replaced by new digits from the left
    };


    NixieDisplay() = default;
    ~NixieDisplay() = default;

    NixieTubeAnimated& operator [](int index);

    void set(const char *p) { set(std::string(p)); }
    void set(const std::string& v);
    void set_effect( NixieTubeAnimated::Effect effect );
    void set_mode( NixieDisplay::Mode mode, NixieDisplay::Mode next_mode = NixieDisplay::Mode::NORMAL);
    void set_random_mode();
    void print();

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
    std::vector<std::string> m_value;
    std::vector<std::string> m_new_value;
    NixieDisplay::Mode m_mode = NixieDisplay::Mode::NORMAL;
    NixieDisplay::Mode m_next_mode = NixieDisplay::Mode::NORMAL;
    int m_mode_step = -1;
    bool m_mode_steps_repeat = false;

    void __set();
    void apply_new_value();
    void do_wrap();
    void do_ordered_wrap();
    void do_ordered_wrap_right_to_left();
    void do_swipe_left();
    void do_swipe_right();
};

