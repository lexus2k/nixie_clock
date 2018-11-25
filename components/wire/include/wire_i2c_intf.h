#pragma once

#include <stdint.h>

#include "driver/i2c.h"

class IWireI2C
{
public:
    virtual bool begin() { return false; };
    virtual void end() {};

    virtual bool beginTransmission(uint8_t address) = 0;
    virtual int write(uint8_t data) = 0;

    /**
     * Requests len bytes from i2c device and stores result to buffer.
     * This method doesn't require endTransmission() call.
     */
    virtual bool requestFrom(uint8_t address, uint8_t *buf, int len) = 0;
    virtual int endTransmission() = 0;
};

