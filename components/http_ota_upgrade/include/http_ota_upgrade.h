#include <stdint.h>

#if !defined(IDF_VERSION)
#include "esp_http_server.h"
#elif IDF_VERSION>=030200
#include "esp_http_server.h"
#else
#include "http_server.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void register_ota_handler( httpd_handle_t server,
                           void (*on_upgrade_start)(void),
                           void (*on_upgrade_end)(bool success) );

#ifdef __cplusplus
}
#endif
