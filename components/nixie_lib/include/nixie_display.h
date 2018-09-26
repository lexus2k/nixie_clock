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

private:
    NixieTube* m_tubes;
    int m_count;
    PinMuxFake m_fakePinMux{};
    NixieTube  m_fakeTube = NixieTube( -1, m_fakePinMux );
};

/*
class NixieDisplay6IN14: public NixieDisplay
{
public:
    NixieDisplay6IN14()
        : NixieDisplay(
    {
        m_muxer.setMap(m_map, sizeof(m_map), 12);
    }

    ~NixieDisplay6IN14()
    {
    }

private:
    PinMuxHv5812 m_muxer( 4 );
    uint8_t m_map[] =
    {
        // 1   2   3   4   5   6   7   8   9   0  ,    ,
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    };
    NixieTube m_tubes[6] =
    {
        
    }

};
*/
