#pragma once

class PinGroupController
{
public:
    virtual void begin() {}
    virtual void end() {}
    virtual void update() {};
    virtual void set(int n) = 0;
    virtual void clear(int n) = 0;
};

