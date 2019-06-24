#include "http_applet_engine.h"

#include <esp_log.h>

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define MAX_READ_PARAM_SIZE  (64)

static const char *TAG="WEB";

bool applet_engine_init(applet_engine_t *engine, void *user_data)
{
    engine->source_buffer = NULL;
    engine->user_data = user_data;
    engine->params = NULL;
    engine->position = NULL;
    engine->end = NULL;
    engine->user_condition_true = false;
    return true;
}

void applet_engine_set_html(applet_engine_t *engine, const char *html, int len)
{
    engine->source_buffer = html;
    engine->end = engine->source_buffer + (len ? len: strlen(html));
    engine->position = engine->source_buffer;
}

void applet_engine_set_params(applet_engine_t *engine, applet_param_t *params)
{
    engine->params = params;
}

static int run_builtin_command(applet_engine_t *engine, const char *command)
{
    if (!strcmp(command, "loop"))
    {
        engine->user_condition_true = false; // By default exit empty cycle
        engine->start_loop_position = engine->position;
        return 1;
    }
    else if (!strcmp(command, "endloop"))
    {
        if ( engine->user_condition_true ) // continue loop
        {
            engine->position  = engine->start_loop_position;
        }
        return 1;
    }
    return 0;
}

static int run_user_command(applet_engine_t *engine, const char *command, char *buffer, int max_len)
{
    if ( engine->params == NULL )
    {
        return -1;
    }
    applet_param_t *p = engine->params;
    int result = -1;
    while (p->name != NULL && p->cb != NULL)
    {
        if ( p->name == NULL || !strcmp(p->name, buffer) )
        {
            applet_callback_data_t data = {
                .name = command,
                .write = false,
                .r = {
                   .data = buffer,
                   .max_len = max_len
                }
            };
            result = p->cb(&data, engine->user_data);
            if ( result == 0 )
            {
                 break;
            }
        }
        p++;
    }
    return result;
}

int applet_engine_process(applet_engine_t *engine, char *buffer, int buffer_size)
{
    int len = 0;
    bool call_found = false;
    while (engine->position < engine->end && len < buffer_size)
    {
        if ( engine->position[0] == '[' && engine->position + 1 < engine->end && engine->position[1] == '%')
        {
            if (len + MAX_READ_PARAM_SIZE < buffer_size )
            {
                call_found = true;
            }
            else
            {
                break;
            }
        }
        if (call_found)
        {
            engine->position += 2;
            const char *p = strchr( engine->position, '%' );
            if (!p)
            {
                ESP_LOGE(TAG, "Failed to parse web page at: %s", engine->position);
            }
            else
            {
                strncpy( buffer, engine->position, p - engine->position );
                buffer[p - engine->position] = '\0';
                engine->position = p + 2;
                if ( run_builtin_command( engine, buffer ) > 0 )
                {
                    // Nothing to do for now
                }
                // Try to run user defined command
                else if ( run_user_command( engine, buffer, buffer, buffer_size - len ) == 0)
                {
                    int user_data_len = strlen(buffer);
                    engine->user_condition_true = user_data_len != 0;
                    len += user_data_len;
                    buffer += user_data_len;
                }
                else
                {
                    engine->user_condition_true = false;
                    ESP_LOGE(TAG, "Failed to get value for: %s", buffer);
                }
            }
            call_found = false;
        }
        else
        {
            *buffer = *(engine->position);
            engine->position++;
            len++;
            buffer++;
        }
    }
    return len;
}

void applet_engine_close(applet_engine_t *engine)
{
}

static const char *source_buffer = NULL;
static const char *position = NULL;
static const char *end = NULL;
static int last_applet_data_len = 0;
static const char *start_loop;

void applet_engine_start(const char *src, int len)
{
    source_buffer = src;
    end = source_buffer + len;
    position = source_buffer;
}

static int run_special_command(const char *command)
{
    if (!strcmp(command, "loop"))
    {
        last_applet_data_len = 0;
        start_loop = position;
        return 1;
    }
    else if (!strcmp(command, "endloop"))
    {
        if ( last_applet_data_len != 0 )
        {
            position  = start_loop;
        }
        return 1;
    }
    return 0;
}

int applet_engine_get_chunk(char *block, int max_len,
                            int (*applet_cb)(const char *applet, char *result, int max_len))
{
    const int min_size = 64;
    int len = 0;
    bool call_found = false;
    while (position < end && len < max_len)
    {
        if ( position[0] == '[' && position + 1 < end && position[1] == '%')
        {
            if (len + min_size < max_len )
            {
                call_found = true;
            }
            else
            {
                break;
            }
        }
        if (call_found)
        {
            position += 2;
            const char *p = strchr( position, '%' );
            if (!p)
            {
                ESP_LOGE(TAG, "Failed to parse web page at: %s", position);
            }
            else
            {
                strncpy( block, position, p - position );
                block[p - position] = '\0';
                position = p + 2;
                if ( run_special_command( block ) > 0 )
                {
                    // Nothing to do for now
                }
                else if (applet_cb( block, block, max_len - len ) == 0)
                {
                    last_applet_data_len = strlen(block);
                    len += last_applet_data_len;
                    block += last_applet_data_len;
                }
                else
                {
                    last_applet_data_len = 0;
                    ESP_LOGE(TAG, "Failed to get value for: %s", block);
                }
            }
            call_found = false;
        }
        else
        {
            *block = *position;
            position++;
            len++;
            block++;
        }
    }
    return len;
}

