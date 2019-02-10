#include "nixie_tube_in12a.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>

void NixieTubeIn12A::set(const char *p)
{
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
}

