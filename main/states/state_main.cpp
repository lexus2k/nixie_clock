#include "state_main.h"
#include "clock_hardware.h"

#include <sys/time.h>
#include <time.h>

void state_main_on_enter(void)
{
}

void state_main_main(void)
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) != 0)
    {
        return;
    }
    char s[7];
    struct tm* tm_info;
    tm_info = localtime(&tv.tv_sec);
    strftime(s, sizeof(s), "%H%M%S", tm_info);
    display.set(s);
}

int state_main_on_event(uint8_t event, uint8_t arg)
{
    return 0;
}

void state_main_on_exit(void)
{
}

