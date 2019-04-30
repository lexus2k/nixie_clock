#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void applet_engine_start(const char *src, int len);

int applet_engine_get_chunk(char *block, int max_len,
                            int (*applet_cb)(const char *applet, char *result, int max_len));

#ifdef __cplusplus
}
#endif
