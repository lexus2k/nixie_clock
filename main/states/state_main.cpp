#include "state_main.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"
#include "sm_engine2.h"

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
    if ( (tm_info->tm_sec & 0x01) == 0 )
    {
        apply_settings();
    }
    if ( m_last_tm_info.tm_min != tm_info->tm_min )
    {
        display.set_effect( NixieTubeAnimated::Effect::SCROLL );
        // display.set_mode( NixieDisplay::Mode::ORDERED_WRAP_RIGHT_TO_LEFT_ONCE );
        // display.set_mode( NixieDisplay::Mode::SWIPE_RIGHT );
        display.set_random_mode();
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

EEventResult StateMain::on_event(SEventData event)
{
    //             from state     event id              event arg      transition_func          type        to state
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     EVT_BUTTON_1,  SM_FUNC_NONE,            SM_PUSH,    CLOCK_STATE_SHOW_TEMP );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     EVT_BUTTON_2,  on_change_highlight(),   SM_NONE,    SM_STATE_NONE );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     EVT_BUTTON_3,  on_highlight_toggle(),   SM_NONE,    SM_STATE_NONE );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_PRESS,     EVT_BUTTON_4,  SM_FUNC_NONE,            SM_PUSH,    CLOCK_STATE_SHOW_IP );
    // Long press events
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_LONG_HOLD, EVT_BUTTON_1,  SM_FUNC_NONE,            SM_SWITCH,  CLOCK_STATE_SETUP_TIME );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_LONG_HOLD, EVT_BUTTON_2,  SM_FUNC_NONE,            SM_SWITCH,  CLOCK_STATE_SETUP_ALARM );
    SM_TRANSITION( SM_STATE_ANY,  EVT_BUTTON_LONG_HOLD, EVT_BUTTON_3,  SM_FUNC_NONE,            SM_PUSH,    CLOCK_STATE_SLEEP );
    // BUTTON 4 long press is processed globally
    return EEventResult::NOT_PROCESSED;
}

void StateMain::on_highlight_toggle()
{
    if (settings.get_highlight_enable())
    {
        settings.set_highlight_enable(false);
        leds.off();
    }
    else
    {
        settings.set_highlight_enable(true);
        leds.on();
    }
}

void StateMain::on_change_highlight()
{
    int color = settings.get_predefined_color() + 1;
    settings.set_predefined_color( color );
    leds.set_color( settings.get_color() );
    leds.set_mode( LedsMode::NORMAL );
}
