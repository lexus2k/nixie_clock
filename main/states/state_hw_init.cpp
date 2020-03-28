#include "state_hw_init.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_events.h"
#include "clock_states.h"
#include "clock_time.h"
#include "http_server_task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "mdns.h"


void StateHwInit::enter()
{
    send_event( SEventData{ EVT_HW_INIT_SUCCESSFUL, 0} );
}

EEventResult StateHwInit::on_event(SEventData event)
{
    //             from state     event id              event arg      transition_func          type        to state
    SM_TRANSITION( SM_STATE_ANY, EVT_HW_INIT_SUCCESSFUL,SM_EVENT_ARG_ANY,  SM_FUNC_NONE,        SM_SWITCH,  CLOCK_STATE_APP_INIT );
    return EEventResult::NOT_PROCESSED;
}
