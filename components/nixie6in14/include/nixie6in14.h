#pragma once

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


class Nixie6in14
{
public:
    Nixie6in14();
    ~Nixie6in14();

    const uint8_t* getData();
    /** in bytes */
    int dataLen();
    void enableDigit(int i, int value);
    void disableDigit(int i, int value);
private:
    uint8_t m_data[6];
    // 6 digits each 12 bits size (0-9 + left comma and right comma)
    uint8_t m_map[6][12] =
    {
        // 1   2   3   4   5   6   7   8   9   0  ,    ,
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        { 12, 13, 14,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        {  0, 12,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        {  0, 13,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        {  0, 14,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
        {  0, 15,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 },
    };
    int getMapIndex(int value);
};
