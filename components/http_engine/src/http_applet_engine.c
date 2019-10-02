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
    return -1;
}

int applet_engine_read_var(applet_engine_t *engine, const char *command, char *buffer, int max_len)
{
    if ( engine->params == NULL )
    {
        return -1;
    }
    applet_param_t *p = engine->params;
    int result = -1;
    while (p->cb_write != NULL || p->cb_read != NULL)
    {
        if ( p->name == NULL || !strcmp(p->name, buffer) )
        {
            result = p->cb_read ? p->cb_read( command, buffer, max_len, engine->user_data )
                                : -1;
            if ( result >= 0 )
            {
                 break;
            }
        }
        p++;
    }
    return result;
}

int applet_engine_write_var(applet_engine_t *engine, const char *command, const char *buffer, int len)
{
    if ( engine->params == NULL )
    {
        return -1;
    }
    applet_param_t *p = engine->params;
    int result = -1;
    while (p->cb_write != NULL || p->cb_write != NULL)
    {
        if ( p->name == NULL || !strcmp(p->name, buffer) )
        {
            result = p->cb_write ? p->cb_write( command, buffer, len, engine->user_data )
                                : -1;
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
                // call is found but we have too small buffer to place param results to
                break;
            }
        }
        if (call_found)
        {
            engine->position += 2; // skip '[%' sequence
            const char *p = strchr( engine->position, '%' );
            if (!p)
            {
                ESP_LOGE(TAG, "Failed to parse web page at: %s", engine->position);
                return -1;
            }
            else
            {
                int user_data_len = -1;
                strncpy( buffer, engine->position, p - engine->position );
                buffer[p - engine->position] = '\0'; // coping parameter name to our buffer temporary to pass to the callbacks
                engine->position = p + 2; // Skip '%]' sequence
                if ( run_builtin_command( engine, buffer ) >= 0 )
                {
                    // Nothing to do for now. We have successfully executed internal command
                }
                // Try to run user defined command
                else if ( (user_data_len = applet_engine_read_var( engine, buffer, buffer, buffer_size - len )) >= 0)
                {
                    // if user data len is zero, trying to process value as null-terminated string
                    user_data_len = user_data_len ?: strlen(buffer);
                    // set condition for loop cycles (zero data len means exit loop)
                    engine->user_condition_true = user_data_len != 0;
                    len += user_data_len;
                    buffer += user_data_len; // shift buffer forward by length of data provided
                }
                else
                {
                    engine->user_condition_true = false; // We failed to read user parameter, set loop condition to false by default
                    ESP_LOGE(TAG, "Failed to get value for: %s", buffer);
                }
            }
            call_found = false;
        }
        else
        {
            // copy single char from source html as is
            *buffer = *(engine->position);
            engine->position++;
            len++;
            buffer++;
        }
    }
    // return number of html data bytes in result buffer
    // if the value is not 0, then process must be called once again
    return len;
}

void applet_engine_close(applet_engine_t *engine)
{
}
