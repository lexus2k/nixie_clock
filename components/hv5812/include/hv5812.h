#pragma once

#include "spibus.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class Hv5812
{
public:
    Hv5812(IWireSPI& spi);

    Hv5812(IWireSPI& spi, gpio_num_t strobe);
    ~Hv5812();

    void setup(gpio_num_t strobe);

    bool begin();
    bool write(const uint8_t *data, int len);
    void end();
private:
    IWireSPI& m_spi;
    gpio_num_t m_strobe;
};

