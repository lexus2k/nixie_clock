#pragma once

#include "hv5812.h"

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
     * Enables specified pin of specified module
     */
    virtual void set(int n, int pin) = 0;

    /**
     * Disables specified pin of specified module
     */
    virtual void clear(int n, int pin) = 0;
private:
};

class PinMuxFake: public PinMux
{
public:
    void set(int n, int pin) override {}
    void clear(int n, int pin) override {};
};

class PinMuxHv5812: public PinMux
{
public:
    PinMuxHv5812(int hv5812_count = 4)
        : m_hv5812_count(hv5812_count)
    {
        memset( m_data, 0xFF, dataLen() );
    }

    void setMap(const uint8_t* pinMap, int size, int pinsPerModule )
    {
        m_map = pinMap;
        m_size = size;
        m_module_pins = pinsPerModule;
    }

    void set(int n, int pin) override
    {
        int index = n * m_module_pins + pin;
        if (index >= m_size)
        {
            return;
        }
        int mapped = m_map[index];
        m_data[mapped/8] &= ~(1 << (mapped & 0x07));
    }

    void clear(int n, int pin) override
    {
        int index = n * m_module_pins + pin;
        if (index >= m_size)
        {
            return;
        }
        int mapped = m_map[index];
        m_data[mapped/8] |= (1 << (mapped & 0x07));
    }

private:
    int m_hv5812_count;
    const uint8_t* m_map = nullptr;
    int m_size = 0;
    int m_module_pins = 0;
    uint8_t m_data[32] = {};

    int dataLen() { return ( m_hv5812_count * 20 + 7 )/8; }
};

    // 6 digits each 12 bits size (0-9 + left comma and right comma)
/*    uint8_t m_map[6][12] =
    {
        // 1   2   3   4   5   6   7   8   9   0  ,    ,
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },
        { 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 },
        { 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 },
        { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59 },
        { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71 },
    }; */
