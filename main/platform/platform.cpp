#include "system.h"
#include "esp_timer.h"


uint32_t micros(void)
{
    return esp_timer_get_time();
}


uint32_t millis(void)
{
    return esp_timer_get_time() / 1000ULL;
}

