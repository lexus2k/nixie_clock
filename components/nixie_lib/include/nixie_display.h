#pragma once

#include "nixie_tube.h"
#include "pin_muxers.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NixieDisplay
{
public:
    NixieDisplay(NixieTube* tubes, int count)
        : m_tubes( tubes )
        , m_count( count ) {};
    ~NixieDisplay() = default;

//    void setDigit(int n, int digit);

    NixieTube& operator [](int index)
    {
        if (index < m_count)
            return m_tubes[index];
        return m_fakeTube;
    };


protected:
    virtual void set(int n, int value) = 0;
    virtual void clear(int n, int value) = 0;
    virtual int get(int n) = 0;
    virtual void brightness(int n, int value) = 0;

private:
    NixieTube* m_tubes;
    int m_count;
    PinMuxFake m_fakePinMux{};
    NixieTube  m_fakeTube = NixieTube( -1, m_fakePinMux );
};

