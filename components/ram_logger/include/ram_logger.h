#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_system.h"
#include "esp_log.h"
#include <string.h>

int ram_logger_init(void);
int ram_logger_vprintf( const char *str, va_list l );
void ram_logger_free(void);
const char *ram_logger_get_logs(void);

#ifdef __cplusplus
}
#endif
