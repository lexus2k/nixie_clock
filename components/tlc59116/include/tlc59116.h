#pragma once

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wire.h"

class Tlc59116
{
public:
    Tlc59116(WireI2C& i2c);
    ~Tlc59116();

    bool begin(uint8_t address);
    void end();

    void enableLeds(uint16_t leds);

    void setGroupBrightness(uint8_t br);
    void setBrightness(uint8_t led, uint8_t brightness);

private:
    WireI2C& m_i2c;
    uint8_t m_address;
};

