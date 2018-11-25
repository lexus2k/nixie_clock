#include "state_main.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_events.h"

#include <sys/time.h>
#include <time.h>

static struct tm last_tm_info{};

static struct tm *get_current_time()
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) != 0)
    {
        return nullptr;
    }
    return localtime(&tv.tv_sec);
}

void state_main_on_enter(void)
{
    last_tm_info = *get_current_time();
}

void state_main_main(void)
{
    char s[16];
    struct tm* tm_info = get_current_time();
    strftime(s, sizeof(s), tm_info->tm_sec & 1 ? "%H.%M.%S " : "%H %M %S ", tm_info);
    if ( last_tm_info.tm_min != tm_info->tm_min )
    {
        if ( tm_info->tm_sec == 0 )
        {
            if ( settings.get_day_time().tm_hour == tm_info->tm_hour &&
                 settings.get_day_time().tm_min == tm_info->tm_min )
            {
                display.set_brightness( settings.get_day_brightness() );
                leds.set_brightness( settings.get_day_brightness() );
            }
            if ( settings.get_night_time().tm_hour == tm_info->tm_hour &&
                 settings.get_night_time().tm_min == tm_info->tm_min &&
                 settings.get_night_mode() )
            {
                display.set_brightness( settings.get_night_brightness() );
                leds.set_brightness( settings.get_night_brightness() );
            }
        }
        display.set_effect( Effect::SCROLL );
        display.set(s);
    }
    else if ( last_tm_info.tm_sec != tm_info->tm_sec )
    {
        display.set_effect( Effect::OVERLAP );
        display.set(s);
    }
    if ( last_tm_info.tm_hour != tm_info->tm_hour)
    {
        update_rtc_chip(false);
    }
    last_tm_info = *tm_info;
}

int state_main_on_event(uint8_t event_id, uint8_t arg)
{
    if ( event_id == EVT_BUTTON_PRESS && arg == BUTTON_1 )
    {
        if (is_power_on())
        {
            power_off();
        }
        else
        {
            power_on();
        }
        return 1;
    }
    return 0;
}

void state_main_on_exit(void)
{
}

