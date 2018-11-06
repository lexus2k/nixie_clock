#pragma once

#include <stdint.h>

class PinGroupController
{
public:
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void update() = 0;
    virtual void set(int n) = 0;
    virtual void set(int n, uint8_t pwm);
    virtual void clear(int n) = 0;
};

