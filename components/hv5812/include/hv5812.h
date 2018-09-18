#pragma once

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class Hv5812
{
public:
    Hv5812(WireSPI& spi);
    ~Hv5812();

    bool begin();
    bool write(const uint8_t *data, int len);
    void end();
private:
    WireSPI& m_spi;
    gpio_num_t m_strobe;
};

