#include "nixie_tube_in12a.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <ctype.h>

const char * NixieTubeIn12A::set(const char *p)
{
    // Digit control
    if (isdigit(*p))
    {
        enable_anod();
        animate( *p -'0' );
        p++;
    }
    else if (*p == ' ')
    {
        disable_anod();
        animate( -1 );
        p++;
    }
    else if (*p == '~' )
    {
        p++;
    }
    return p;
}
