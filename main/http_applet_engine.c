#include "http_applet_engine.h"

#include <esp_log.h>

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

static const char *TAG="WEB";

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

