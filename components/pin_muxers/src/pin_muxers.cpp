#include "nixie6in14.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"


Nixie6in14::Nixie6in14()
{
    memset(m_data, 0xFF, sizeof(m_data));
}

Nixie6in14::~Nixie6in14()
{
}

void Nixie6in14::enableDigit(int i, int value)
{
    int pin = m_map[i][getMapIndex(value)];
    m_data[pin >> 3] &= ~(1 << (pin & 0x07));
}

void Nixie6in14::disableDigit(int i, int value)
{
    int pin = m_map[i][getMapIndex(value)];
    m_data[pin >> 3] |= (1 << (pin & 0x07));
}

int getMapIndex(int value)
{
    if (value == 0)
        return 9;
    if (value < 10)
        return value - 1;
    if (value < 12)
        return value;
    return 0;
}

const uint8_t* Nixie6in14::getData()
{
    return m_data;
}

int Nixie6in14::dataLen()
{
    return sizeof(m_data);
}
