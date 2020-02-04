#pragma once

#include <stdint.h>

class IWireSPI
{
public:
    virtual bool begin() { return false; }
    virtual void end() {}

    virtual bool beginTransaction(uint32_t freq, int cs, int mode) = 0;
    virtual void endTransaction() = 0;
    virtual uint8_t transfer(uint8_t byte) = 0;
    virtual int transfer( const uint8_t *data, int len) = 0;
};

