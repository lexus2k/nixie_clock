#include "in12tubes.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>

const char * NixieTubeIn12A_Dots::set(const char *p, bool apply)
{
    // Left dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',' || *p ==' ' || *p =='~' )
        {
            p++;
        }
    }
    // Digit
    p = NixieTubeIn12A::set(p, apply);
    // Right dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',')
        {
            if (apply)
            {
                enable_cathode(10);
            }
            p++;
        }
        else if (*p == ' ')
        {
            if (apply)
            {
                disable_cathode(10);
            }
            p++;
        }
        else if (*p =='~')
        {
            p++;
        }
    }
    else
    {
        if (apply)
        {
            disable_cathode(10);
        }
    }
    return p;
}

const char * NixieTubeIn12A_NoDots::set(const char *p, bool apply)
{
    // Left dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',' || *p ==' ' || *p =='~' )
        {
            p++;
        }
    }
    // Digit
    p = NixieTubeIn12A::set(p, apply);
    // Right dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',' || *p ==' ' || *p =='~' )
        {
            p++;
        }
    }
    return p;
}
