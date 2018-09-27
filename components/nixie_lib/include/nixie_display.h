#pragma once

#include "nixie_tube.h"
#include "pin_muxers.h"
#include <stdint.h>
#include <string.h>
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieDisplay
{
public:
    NixieDisplay() = default;
    ~NixieDisplay() = default;

    NixieTube& operator [](int index)
    {
        NixieTube* tube = get_by_index(index);
        if (tube == nullptr)
        {
            tube = &m_fakeTube;
        }
        return *tube;
    };

    void set_pin_muxer(PinMux* muxer);

    void set_anods(gpio_num_t* pins);

    void enable_pwm(ledc_channel_t* channel, ledc_timer_t timer = LEDC_TIMER_0);

    void begin();
    void end();
    void update();

protected:
    virtual NixieTube* get_by_index(int index) = 0;

private:
    PinMuxFake m_fakePinMux{};
    NixieTube  m_fakeTube = NixieTube( -1, &m_fakePinMux );
    PinMux* m_pin_muxer = nullptr;
};



class NixieDisplay6IN14: public NixieDisplay
{
public:
    NixieDisplay6IN14( ): NixieDisplay()
    {
    }

    ~NixieDisplay6IN14()
    {
    }

protected:
    NixieTube* get_by_index(int index) override
    {
        if (index < sizeof(m_tubes) / sizeof(m_tubes[0]))
            return &m_tubes[index];
        return nullptr;
    }

private:
    NixieTube m_tubes[6] = { {0}, {1}, {2}, {3}, {4}, {5} };
};
