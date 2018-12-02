#include "state_main.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "state_engine.h"

#include <sys/time.h>
#include <time.h>

static struct tm *get_current_time()
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) != 0)
    {
        return nullptr;
    }
    return localtime(&tv.tv_sec);
}

void StateMain::enter()
{
    m_last_tm_info = *get_current_time();
}

void StateMain::run()
{
    char s[16];
    struct tm* tm_info = get_current_time();
    strftime(s, sizeof(s), tm_info->tm_sec & 1 ? "%H.%M.%S " : "%H %M %S ", tm_info);
    if ( m_last_tm_info.tm_min != tm_info->tm_min )
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
    else if ( m_last_tm_info.tm_sec != tm_info->tm_sec )
    {
        display.set_effect( Effect::OVERLAP );
        display.set(s);
    }
    if ( m_last_tm_info.tm_hour != tm_info->tm_hour)
    {
        update_rtc_chip(false);
    }
    m_last_tm_info = *tm_info;
}

bool StateMain::on_event(SEventData event)
{
    if ( event.event == EVT_BUTTON_PRESS && event.arg == BUTTON_1 )
    {
        if (is_power_on())
        {
            power_off();
        }
        else
        {
            power_on();
        }
        return true;
    }
    return false;
}

uint8_t StateMain::get_id()
{
    return CLOCK_STATE_MAIN;
}
