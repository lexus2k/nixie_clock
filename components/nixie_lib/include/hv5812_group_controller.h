#pragma once

#include "pin_group_controller.h"
#include "pin_muxers.h"
#include "hv5812.h"
#include "spibus.h"

#include <vector>

class PinGroupControllerHv5812: public PinGroupController
{
public:
    PinGroupControllerHv5812(WireSPI &spi);
    PinGroupControllerHv5812(WireSPI &spi, gpio_num_t strobe);
    ~PinGroupControllerHv5812() {};

    void setup(gpio_num_t strobe, const std::vector<int>& pin_map = std::vector<int>(0));

    /**
     * Negative value in pin map means no pin is assigned to that position
     */
    void set_map(const std::vector<int>& pin_map);

    void begin() override;

    void end() override;

    void update() override;

    void set(int pin) override;

    void clear(int pin) override;

private:
    Hv5812  m_hv5812;
    int m_hv5812_count;
    std::vector<int> m_map;
    int m_size_bits = 0;
    std::vector<uint8_t> m_data;
};

