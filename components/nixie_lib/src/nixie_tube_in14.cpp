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
    uint8_t state = STATE_LEFT_DOT;
    while (*p)
    {
        if (state == STATE_LEFT_DOT)
        {
            if (isdigit(*p))
            {
            }
            else if (*p == '.' || *p == ',')
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
            state = STATE_DIGIT;
        }
        else if (state == STATE_DIGIT)
        {
            if (isdigit(*p))
            {
                animate( *p -'0' );
                p++;
            }
            else if (*p == ' ')
            {
                animate( -1 );
                p++;
            }
            else if (*p == '~' )
            {
                p++;
            }
            state = STATE_RIGHT_DOT;
        }
        else if (state == STATE_RIGHT_DOT)
        {
            if (isdigit(*p))
            {
                break;
            }
            else if (*p == '.' || *p == ',')
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
            break;
        }
        else
        {
            break;
        }
    }
    return p;
}
