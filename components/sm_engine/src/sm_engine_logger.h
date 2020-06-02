#pragma once

#if defined(__XTENSA__)
#include "esp_log.h"
#else


#define ESP_LOGI(...)
#define ESP_LOGD(...)
#define ESP_LOGW(...)
#define ESP_LOGE(...)


#endif

