#include "nixie_tube_in3.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

const char * NixieTubeIn3::set(const char * p)
{
    if (*p)
    {
        if ((*p == '.') || (*p == ','))
        {
            enable_cathode( 0 );
            enable_anod();
            p++;
        }
        if (*p == ' ')
        {
            disable_cathode( 0 );
            disable_anod();
            p++;
        }
        if (*p == '~')
        {
            p++;
        }
    }
    return p;
}
