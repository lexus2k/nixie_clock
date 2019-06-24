#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct
{
    const char *name;
    bool write;
    union
    {
        struct
        {
            char *data;
            int len;
        } w;
        struct
        {
            char *data;
            int max_len;
        } r;
    };
} applet_callback_data_t;

typedef struct
{
    const char *name; // Can be empty to process params by default
    int (*cb)(applet_callback_data_t *data, void *user_data);
} applet_param_t;

typedef struct
{
    const char *source_buffer;
    void *user_data;
    applet_param_t *params;
    // Internal below
    const char *position;
    const char *end;
    bool user_condition_true;
    const char *start_loop_position;
} applet_engine_t;

bool applet_engine_init(applet_engine_t *engine, void *user_data);
void applet_engine_set_html(applet_engine_t *engine, const char *html, int len);
void applet_engine_set_params(applet_engine_t *engine, applet_param_t *params);
int applet_engine_process(applet_engine_t *engine, char *buffer, int buffer_size);
void applet_engine_close(applet_engine_t *engine);

void applet_engine_start(const char *src, int len);
int applet_engine_get_chunk(char *block, int max_len,
                            int (*applet_cb)(const char *applet, char *result, int max_len));

#ifdef __cplusplus
}
#endif
