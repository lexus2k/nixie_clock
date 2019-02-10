#include "nixie_tube_in14.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>

void NixieTubeIn14::set(const char *p)
{
    // Left dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',')
        {
            enable_cathode(10);
        }
        else if (*p == ' ')
        {
            disable_cathode(10);
        }
    }
    p++;
    // Digit control
    if (isdigit(*p))
    {
        enable_anod();
        animate( *p -'0' );
    }
    else if (*p == ' ')
    {
        disable_anod();
        animate( -1 );
    }
    p++;
    // Right dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',')
        {
            enable_cathode(11);
        }
        else if (*p == ' ')
        {
            disable_cathode(11);
        }
    }
}
