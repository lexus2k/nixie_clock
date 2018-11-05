#pragma once

#include "pin_group_controller.h"
#include "pin_muxers.h"
#include "hv5812.h"
#include "spibus.h"

class PinGroupControllerHv5812: public PinGroupController
{
public:
    PinGroupControllerHv5812(WireSPI &spi, gpio_num_t strobe, int hv5812_count = 4);
    ~PinGroupControllerHv5812() {};

    /**
     * Negative value in pin map means no pin is assigned to that position
     */
    void set_map(const int* pinMap, int size);

    void begin() override;

    void end() override;

    void update() override;

    void set(int pin) override;

    void clear(int pin) override;

private:
    Hv5812  m_hv5812;
    int m_hv5812_count;
    const int* m_map = nullptr;
    int m_size_bits = 0;
    uint8_t m_data[32] = {};

    int dataLen() { return ( m_hv5812_count * 20 + 7 )/8; }
};

