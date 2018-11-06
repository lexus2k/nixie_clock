#pragma once

#include "nixie_tube.h"
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
    NixieDisplay() = default;
    ~NixieDisplay() = default;

    NixieTubeAnimated& operator [](int index)
    {
        NixieTubeAnimated* tube = get_by_index(index);
        if (tube == nullptr)
        {
            tube = &m_fake_tube;
        }
        return *tube;
    };

    void set(const char *p);
    void scroll(const char *p);
    void overlap(const char *p);

    void set_anods(gpio_num_t* pins);

    void enable_pwm(ledc_channel_t* channel, ledc_timer_t timer = LEDC_TIMER_0);
    void on();
    void set_brightness(uint8_t brightness);

    virtual void begin();
    virtual void end();
    virtual void update();

    void set_pwm_range(uint16_t min_pwm, uint16_t max_pwm );

protected:
    virtual NixieTubeAnimated* get_by_index(int index) = 0;
    void do_for_each(const std::function<void(NixieTubeAnimated &tube)> &func);

private:
    NixieTube  m_fake_tube;
};

