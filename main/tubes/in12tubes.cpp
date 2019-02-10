#include "in12tubes.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>

void NixieTubeIn12A_Dots::set(const char *p)
{
    // Digit
    NixieTubeIn12A::set(p);
    p += 2;
    // Right dot control
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
    else
    {
        disable_cathode(10);
    }
}

void NixieTubeIn12A_NoDots::set(const char *p)
{
    NixieTubeIn12A::set(p);
}
