#pragma once

#include "esp_http_server.h"
#include "esp_https_server.h"

#ifdef __cplusplus
extern "C" {
#endif

int form_auth_server_generate_token(char *token, int max_len, const char *user, const char *password);
uint8_t form_auth_server_validate_token(const char *token, const char *user, const char *password);

#ifdef __cplusplus
}
#endif
