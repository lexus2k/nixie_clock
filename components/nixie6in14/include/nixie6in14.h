#pragma once

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class PinMux
{
public:
    PinMux() = default;
    ~PinMux() = default;

    /**
     * Enables specified pin of specified tube
     */
    virtual void set(int n, int pin) = 0;

    /**
     * Disables specified pin of specified tube
     */
    virtual void clear(int n, int pin) = 0;
private:
};

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

    void begin();
    void end();

protected:
    int m_index;
    PinMux& m_pinmux;
    int m_pin;
    bool m_enabled = false;
    int m_digit = 0;
};

void NixieTube::begin()
{
    if (m_pin >= 0)
    {
        gpio_set_direction(static_cast<gpio_num_t>(m_pin), GPIO_MODE_OUTPUT);
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::end()
{
    if (m_pin >= 0)
    {
        gpio_set_level(static_cast<gpio_num_t>(m_pin), 0);
    }
}

void NixieTube::set(int digit)
{
    m_pinmux.clear(m_index, m_digit);
    m_digit = digit;
    m_pinmux.set(m_index, m_digit);
}

/*
class NixieDisplay
{
public:
    NixieDisplay() = default;
    ~NixieDisplay() = default;

    void setDigit(int n, int digit);

protected:
    virtual void set(int n, int value) = 0;
    virtual void clear(int n, int value) = 0;
    virtual int get(int n) = 0;
    virtual void brightness(int n, int value) = 0;
};


void NixieDisplay::setDigit(int n, int digit)
{
}
*/

class Nixie6in14
{
public:
    Nixie6in14();
    ~Nixie6in14();

    const uint8_t* getData();
    /** in bytes */
    int dataLen();
    void enableDigit(int i, int value);
    void disableDigit(int i, int value);
private:
    uint8_t m_data[9];
    // 6 digits each 12 bits size (0-9 + left comma and right comma)
    uint8_t m_map[6][12] =
    {
        // 1   2   3   4   5   6   7   8   9   0  ,    ,
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },
        { 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 },
        { 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 },
        { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 },
        { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 },
    };
    int getMapIndex(int value);
};
