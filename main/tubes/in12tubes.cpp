#include "in12tubes.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>

const char * NixieTubeIn12A_Dots::set(const char *p)
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
    p = NixieTubeIn12A::set(p);
    // Right dot control
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
    else
    {
        disable_cathode(10);
    }
    return p;
}

const char * NixieTubeIn12A_NoDots::set(const char *p)
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
    p = NixieTubeIn12A::set(p);
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
