#pragma once

#include "pin_muxers.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieTube
{
public:
    NixieTube(int index, PinMux& mux)
        : m_index(index)
        , m_pinmux(mux)
        , m_pin(-1) {};
    NixieTube(int index, gpio_num_t anod, PinMux& mux)
        : m_index(index)
        , m_pinmux(mux)
        , m_pin(anod) {};
    ~NixieTube() = default;

    void set(int digit);
    void on();
    void off();

    void update();

    void begin();
    void end();

protected:
    int m_index;
    PinMux& m_pinmux;
    int m_pin;
    bool m_enabled = false;
    int m_digit = 0;
};

