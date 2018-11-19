#include "nixie_tube_in14.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <ctype.h>

enum
{
    STATE_LEFT_DOT,
    STATE_DIGIT,
    STATE_RIGHT_DOT,
};

const char * NixieTubeIn14::set(const char *p)
{
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',')
        {
            enable_cathode(10);
            p++;
        }
        else if (*p == ' ')
        {
            disable_cathode(10);
            p++;
        }
        else if (*p =='~')
        {
            p++;
        }
    }
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
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',')
        {
            enable_cathode(11);
            p++;
        }
        else if (*p == ' ')
        {
            disable_cathode(11);
            p++;
        }
        else if (*p =='~')
        {
            p++;
        }
    }
    return p;
}
