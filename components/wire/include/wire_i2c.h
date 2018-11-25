#pragma once

#include "wire_i2c_intf.h"

#include <stdint.h>
#include "driver/i2c.h"

class WireI2C: public IWireI2C
{
public:
    WireI2C(i2c_port_t bus = I2C_NUM_1, uint32_t clock=0);
    ~WireI2C();
    bool begin() override;
    void end() override;

    bool beginTransmission(uint8_t address) override;
    int write(uint8_t data) override;

    /**
     * Requests len bytes from i2c device and stores result to buffer.
     * This method doesn't require endTransmission() call.
     */
    bool requestFrom(uint8_t address, uint8_t *buf, int len) override;
    int endTransmission() override;

private:
    i2c_port_t m_bus;
    uint32_t m_clock;
    i2c_cmd_handle_t m_handle;
    uint8_t m_address;
};

