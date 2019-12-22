#include "state_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_states.h"
#include "clock_time.h"
#include "http_server_task.h"

#include "esp_log.h"

void StateInit::enter()
{
    char s[CLOCK_TIME_FORMAT_SIZE];
    display.set(get_time_str(s,sizeof(s),nullptr));
    apply_settings();
    display.on();
}

void StateInit::run()
{
    if ( timeout_event( 1000000 ) )
    {
         switch_state( CLOCK_STATE_MAIN );
    }
}
