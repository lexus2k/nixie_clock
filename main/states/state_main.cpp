#include "state_main.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_time.h"
#include "clock_states.h"
#include "clock_events.h"
#include "clock_buttons.h"

#include <sys/time.h>
#include <time.h>

enum DisplayContent
{
    C_TIME,
    C_DATE,
    C_TEMP,
};

static struct tm *get_current_time()
{
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) != 0)
    {
        return nullptr;
    }
    return localtime(&tv.tv_sec);
}

void StateMain::enter(SEventData *event)
{
    m_last_tm_info = *get_current_time();
    m_content = C_TIME;
    m_cooldown_alarm = false;
    m_forceRefresh = true;
}

void StateMain::update()
{
    if ( m_content == C_TIME )
    {
        if ( timeoutEvent( 10 * 1000000, false ) )
        {
            resetTimeout();
            display.set_mode(NixieDisplay::Mode::SWIPE_LEFT);
            m_forceRefresh = true;
            m_content = C_TEMP;
        }
    }
    else if ( m_content == C_TEMP )
    {
        if ( timeoutEvent( 5 * 1000000, false ) )
        {
            resetTimeout();
            display.set_mode(NixieDisplay::Mode::SWIPE_RIGHT);
            m_forceRefresh = true;
            m_content = C_TIME;
        }
    }
    updateDisplay();
    if ( (m_last_tm_info.tm_sec & 0x01) == 0 )
    {
        apply_settings();
    }
    if ( (m_last_tm_info.tm_sec == 00 ) &&
         (m_last_tm_info.tm_min == (settings.get_alarm() & 0xFF)) &&
         (m_last_tm_info.tm_hour == ((settings.get_alarm() >> 8) & 0xFF) ) &&
         ((settings.get_alarm() >> 24) != 0) && !m_cooldown_alarm )
    {
        sendEvent( SEventData{ EVT_ALARM_ON, 0} );
        m_cooldown_alarm = true;
    }
}

STransitionData StateMain::onEvent(SEventData event)
{
    //                 event id              event arg      transition_func          to state
    TRANSITION_PUSH(   EVT_BUTTON_PRESS,     EVT_BUTTON_1,  sme::NO_FUNC(),            CLOCK_STATE_SHOW_TEMP )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_2,  on_change_highlight() )
    NO_TRANSITION(     EVT_BUTTON_PRESS,     EVT_BUTTON_3,  on_highlight_toggle() )
    TRANSITION_PUSH(   EVT_BUTTON_PRESS,     EVT_BUTTON_4,  sme::NO_FUNC(),            CLOCK_STATE_SHOW_IP )
    // Long press events
    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_1,  sme::NO_FUNC(),            CLOCK_STATE_SETUP_TIME )
    TRANSITION_SWITCH( EVT_BUTTON_LONG_HOLD, EVT_BUTTON_2,  sme::NO_FUNC(),            CLOCK_STATE_SETUP_ALARM )
    TRANSITION_PUSH(   EVT_BUTTON_LONG_HOLD, EVT_BUTTON_3,  sme::NO_FUNC(),            CLOCK_STATE_SLEEP )

    TRANSITION_PUSH(   EVT_ALARM_ON,     SM_EVENT_ARG_ANY,  sme::NO_FUNC(),            CLOCK_STATE_ALARM )
    // BUTTON 4 long press is processed globally
    TRANSITION_TBL_END
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

void StateMain::updateDisplay()
{
    struct tm* tm_info = get_current_time();
    if ( m_last_tm_info.tm_hour != tm_info->tm_hour)
    {
        update_rtc_chip(false);
    }

    if (m_content == C_TIME)
    {
        char s[CLOCK_TIME_FORMAT_SIZE];
        get_time_str(s, sizeof(s), tm_info);
        if ( m_last_tm_info.tm_min != tm_info->tm_min && !m_forceRefresh )
        {
            display.set_effect( NixieTubeAnimated::Effect::SCROLL );
            // display.set_mode( NixieDisplay::Mode::ORDERED_WRAP_RIGHT_TO_LEFT_ONCE );
            // display.set_mode( NixieDisplay::Mode::SWIPE_RIGHT );
            display.set_random_mode();
            display.set(s);
        }
        else if ( m_last_tm_info.tm_sec != tm_info->tm_sec || m_forceRefresh )
        {
            if ( !m_forceRefresh )
            {
                display.set_effect( NixieTubeAnimated::Effect::OVERLAP );
            }
            display.set(s);
        }
    }
    else if (m_content == C_TEMP)
    {
        if ( m_last_tm_info.tm_sec != tm_info->tm_sec || m_forceRefresh )
        {
            char str[18];
            int temp = temperature.get_celsius_hundreds();
            snprintf(str, sizeof(str),"       %d  %d. %d  %d ",
                (temp/1000),
                (temp/100)%10,
                (temp/10)%10,
                (temp/1)%10);
            display.set(str);
        }
    }
    m_last_tm_info = *tm_info;
    m_forceRefresh = false;
}
