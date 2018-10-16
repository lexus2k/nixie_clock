#pragma once

#ifdef __linux__

#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#define ESP_LOGE( x, y, ...)    \
   { \
       fprintf(stderr, "\n[%s][ERR]",x); \
       fprintf(stderr, y, ##__VA_ARGS__ ); \
   }

#define ESP_LOGW( x, y, ...)    \
   { \
       fprintf(stderr, "\n[%s][WRN]",x); \
       fprintf(stderr, y, ##__VA_ARGS__ ); \
   }

#define ESP_LOGI( x, y, ...)    \
   { \
       fprintf(stderr, "\n[%s][INF]",x); \
       fprintf(stderr, y, ##__VA_ARGS__ ); \
   }

#define ESP_LOGD( x, y, ...)    \
   { \
       fprintf(stderr, "\n[%s][DBG]",x); \
       fprintf(stderr, y, ##__VA_ARGS__ ); \
   }

#define ESP_LOG_BUFFER_HEX_LEVEL(...)

#else

#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#endif


