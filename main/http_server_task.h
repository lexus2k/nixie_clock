#pragma once


#if !defined(IDF_VERSION)
#include "esp_http_server.h"
#elif IDF_VERSION>=030200
#include "esp_http_server.h"
#else
#include "http_server.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void start_webserver(void);
void stop_webserver(void);

#ifdef __cplusplus
}
#endif
