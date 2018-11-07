#include "nixie_tube_in14.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

/*
void NixieTubeIn14::set(const char* str)
{
    int offset = 0;
    while (*str)
    {
        if (*str >= '0' && *str <='9')
        {
            disable_cathode( m_value );
            m_value = (*str - '0');
            enable_cathode( m_value );
        }
        
    }
}
*/