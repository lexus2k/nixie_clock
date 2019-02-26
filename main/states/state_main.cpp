#include "state_main.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "nixie_clock.h"

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
    char s[CLOCK_TIME_FORMAT_SIZE];
    struct tm* tm_info = get_current_time();
    get_time_str(s, sizeof(s), tm_info);
    if ( (tm_info->tm_sec & 0x03) == 0 )
    {
        apply_settings();
    }
    if ( m_last_tm_info.tm_min != tm_info->tm_min )
    {
        display.set_effect( NixieTubeAnimated::Effect::SCROLL );
        display.set_mode( NixieDisplay::Mode::ORDERED_WRAP_RIGHT_TO_LEFT_ONCE );
        display.set(s);
    }
    else if ( m_last_tm_info.tm_sec != tm_info->tm_sec )
    {
        display.set_effect( NixieTubeAnimated::Effect::OVERLAP );
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
        push_state(CLOCK_STATE_SLEEP);
        return true;
    }
    if ( event.event == EVT_BUTTON_PRESS && event.arg == BUTTON_2 )
    {
        if (settings.get_highlight_enable())
        {
            settings.set_highlight_enable(false);
            leds.disable();
        }
        else
        {
            settings.set_highlight_enable(true);
            leds.enable();
        }
    }
    if ( event.event == EVT_BUTTON_PRESS && event.arg == BUTTON_3 )
    {
        int color = settings.get_predefined_color() + 1;
        settings.set_predefined_color( color );
        leds.set_color( settings.get_color() );
    }
    if ( event.event == EVT_BUTTON_LONG_HOLD && event.arg == BUTTON_3 )
    {
        push_state( CLOCK_STATE_SHOW_TEMP );
    }
    return false;
}

uint8_t StateMain::get_id()
{
    return CLOCK_STATE_MAIN;
}
