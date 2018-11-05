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

class PinMuxHv5812: public PinMux
{
public:
    PinMuxHv5812(WireSPI& spi, gpio_num_t strobe, int hv5812_count = 4);

    /**
     * Negative value in pin map means no pin is assigned to that position
     */
    void set_map(const int* pinMap, int size, int pinsPerModule );

    void begin() override;

    void end() override;

    void update() override;

    void set(int n, int pin) override;

    void clear(int n, int pin) override;

private:
    Hv5812  m_hv5812;
    int m_hv5812_count;
    const int* m_map = nullptr;
    int m_size_bits = 0;
    int m_module_pins = 0;
    uint8_t m_data[32] = {};

    int dataLen() { return ( m_hv5812_count * 20 + 7 )/8; }
};

