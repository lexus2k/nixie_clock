#pragma once

#include "hv5812.h"
#include "spibus.h"

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

    /**
     * Commit changes to hardware
     */
    virtual void update() {}

    virtual void begin() {}

    virtual void end() {}
private:
};

class PinMuxFake: public PinMux
{
public:
    void set(int n, int pin) override {}
    void clear(int n, int pin) override {}
};

#include <stdio.h>

class PinMuxHv5812: public PinMux
{
public:
    PinMuxHv5812(WireSPI& spi, gpio_num_t strobe, int hv5812_count = 4)
        : m_hv5812(spi, strobe)
        , m_hv5812_count(hv5812_count)
    {
        memset( m_data, 0xFF, sizeof(m_data) );
    }

    void set_map(const uint8_t* pinMap, int size, int pinsPerModule )
    {
        m_map = pinMap;
        m_size_bits = size;
        m_module_pins = pinsPerModule;
    }

    void begin() override { m_hv5812.begin(); }

    void end() override { m_hv5812.end(); }

    void update() override { m_hv5812.write(m_data, dataLen()); }

    void set(int n, int pin) override
    {
        int index = n * m_module_pins + pin;
        if (index >= m_size_bits)
        {
            return;
        }
        int mapped = m_map[index];
        m_data[ dataLen() - 1 - mapped/8] &= ~(1 << (mapped & 0x07));
    }

    void clear(int n, int pin) override
    {
        int index = n * m_module_pins + pin;
        if (index >= m_size_bits)
        {
            return;
        }
        int mapped = m_map[index];
        m_data[ dataLen() - 1 - mapped/8] |= (1 << (mapped & 0x07));
    }

private:
    Hv5812  m_hv5812;
    int m_hv5812_count;
    const uint8_t* m_map = nullptr;
    int m_size_bits = 0;
    int m_module_pins = 0;
    uint8_t m_data[32] = {};

    int dataLen() { return ( m_hv5812_count * 20 + 7 )/8; }
};

