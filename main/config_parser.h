#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// modifies passed content
int apply_new_config(char *data, int len);
int get_config_value(const char *param, char *data, int max_len);

#ifdef __cplusplus
}
#endif

