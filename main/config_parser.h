#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// modifies passed content
int apply_new_config(char *data, int len);

#ifdef __cplusplus
}
#endif

