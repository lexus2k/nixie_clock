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
    switch_state(CLOCK_STATE_APP_INIT);
}
