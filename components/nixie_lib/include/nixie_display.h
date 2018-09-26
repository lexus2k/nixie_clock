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
    NixieDisplay(NixieTube* const* tubes, int count)
        : __m_tubes( tubes )
        , m_count( count ) {};
    ~NixieDisplay() = default;

//    void setDigit(int n, int digit);

    NixieTube& operator [](int index)
    {
        if (index < m_count)
            return *__m_tubes[index];
        return m_fakeTube;
    };

    void set_pin_muxer(PinMux* muxer)
    {
        for (int i=0; i<m_count; i++)
        {
            __m_tubes[i]->set_pin_muxer( muxer );
        }
    }

private:
    NixieTube* const* __m_tubes;
    int m_count;
    PinMuxFake m_fakePinMux{};
    NixieTube  m_fakeTube = NixieTube( -1, -1, &m_fakePinMux );
};


/*
class NixieDisplay6IN14: public NixieDisplay
{
public:
    NixieDisplay6IN14( m_tubes )
        : NixieDisplay(
    {
    }

    ~NixieDisplay6IN14()
    {
    }

private:
    NixieTube m_tubes[6] =
    {
    }

};

*/
