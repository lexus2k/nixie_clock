#include "nixie_tube_in14.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>

enum
{
    STATE_LEFT_DOT,
    STATE_DIGIT,
    STATE_RIGHT_DOT,
};

const char * NixieTubeIn14::set(const char *p, bool apply)
{
    // Left dot control
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
    // Digit control
    if (isdigit(*p))
    {
        if (apply)
        {
            enable_anod();
            animate( *p -'0' );
        }
        p++;
    }
    else if (*p == ' ')
    {
        if (apply)
        {
            disable_anod();
            animate( -1 );
        }
        p++;
    }
    else if (*p == '~' )
    {
        p++;
    }
    // Right dot control
    if (!isdigit(*p))
    {
        if (*p == '.' || *p == ',')
        {
            if (apply)
            {
                enable_cathode(11);
            }
            p++;
        }
        else if (*p == ' ')
        {
            if (apply)
            {
                disable_cathode(11);
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
            disable_cathode(11);
        }
    }
    return p;
}
