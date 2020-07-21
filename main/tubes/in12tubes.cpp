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

std::string NixieTubeIn12A_Dots::get_content()
{
    int digit = get_enabled_cathode();
    int rdot = get_enabled_cathode( 9 );
    if ( digit > 9 )
    {
        digit = -1;
    }
    std::string result = "";
    result += ' ';
    result += digit < 0 ? ' ': ('0' + digit);
    result += rdot < 0 ? ' ': '.';
    return result;
}

void NixieTubeIn12A_NoDots::set(const char *p)
{
    NixieTubeIn12A::set(p);
}


std::string NixieTubeIn12A_NoDots::get_content()
{
    int digit = get_enabled_cathode();
    if ( digit > 9 )
    {
        digit = -1;
    }
    std::string result = "";
    result += ' ';
    result += digit < 0 ? ' ': ('0' + digit);
    result += ' ';
    return result;
}
