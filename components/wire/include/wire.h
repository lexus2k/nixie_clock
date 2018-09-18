#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void wire_init(int8_t busId);

void wire_start(uint8_t addr, int write_mode);

void wire_stop(void);

void wire_send(uint8_t data);

void wire_close(void);

#ifdef __cplusplus
}

#include "driver/i2c.h"

class WireI2C
{
public:
    WireI2C(i2c_port_t bus = I2C_NUM_1, uint32_t clock=0);
    ~WireI2C();
    bool begin();
    void end();

    bool beginTransmission(uint8_t address);
    int write(uint8_t data);

    bool requestFrom(uint8_t address, int len);
    uint8_t read();

    int endTransmission();
private:
    i2c_port_t m_bus;
    uint32_t m_clock;
    i2c_cmd_handle_t m_handle;
};

#endif
