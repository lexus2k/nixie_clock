#include "state_main.h"
#include "clock_display.h"
#include "clock_hardware.h"

#include <sys/time.h>
#include <time.h>

void state_main_on_enter(void)
{
}

void state_main_main(void)
{
    static struct tm last_tm_info{};
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) != 0)
    {
        return;
    }
    char s[16];
    struct tm* tm_info;
    tm_info = localtime(&tv.tv_sec);
    strftime(s, sizeof(s), tm_info->tm_sec & 1 ? "%H.%M.%S " : "%H %M %S ", tm_info);
    if ( last_tm_info.tm_min != tm_info->tm_min )
    {
        display.set_effect( Effect::SCROLL );
        display.set(s);
    }
    else if ( last_tm_info.tm_sec != tm_info->tm_sec )
    {
        display.set_effect( Effect::OVERLAP );
        display.set(s);
    }
    last_tm_info = *tm_info;
}

int state_main_on_event(uint8_t event, uint8_t arg)
{
    return 0;
}

void state_main_on_exit(void)
{
}

