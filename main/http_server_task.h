#pragma once

#include "esp_http_server.h"
#include "esp_https_server.h"

#ifdef __cplusplus
extern "C" {
#endif

void start_webserver(void);
void stop_webserver(void);

#ifdef __cplusplus
}
#endif
