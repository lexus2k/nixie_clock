/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "ram_logger.h"

#include "esp_system.h"
#include "esp_log.h"

#include <string.h>

static const int MINIMUM_SHIFT = 256;
static const char *TAG = "RAM_LOG";
static vprintf_like_t old_function = NULL;
static char *text_buffer = NULL;
static int buffer_size = 0;
static int position = 0;

int ram_logger_vprintf( const char *str, va_list l );

int ram_logger_init(int ram_size)
{
    buffer_size = ram_size > MINIMUM_SHIFT ? ram_size : 2048;
    position = 0;
    text_buffer = malloc( buffer_size );
    if ( text_buffer == NULL )
    {
        ESP_LOGE( TAG, "Failed to allocate memory" );
        return -1;
    }
    memset( text_buffer, 0, buffer_size );
    ESP_LOGI( TAG, "Enabled" );
    old_function = esp_log_set_vprintf(&ram_logger_vprintf);
    return 0;
}

int ram_logger_vprintf( const char *str, va_list l )
{
    int bytes_available = buffer_size - position;
    int len = vsnprintf(text_buffer + position, bytes_available, str, l);
    if ( len + 1 >= bytes_available )
    {
        memmove( text_buffer, text_buffer + MINIMUM_SHIFT, buffer_size - MINIMUM_SHIFT );
        position -= MINIMUM_SHIFT;
        bytes_available = buffer_size - position;
        len = vsnprintf(text_buffer + position, bytes_available, str, l);
    }
    if ( len > 0 )
    {
        position += len;
    }
    return old_function( str, l );
}

void ram_logger_free(void)
{
    esp_log_set_vprintf( old_function );
    free( text_buffer );
    text_buffer = NULL;
    ESP_LOGI(TAG, "disabled");
}

const char *ram_logger_get_logs(void)
{
    return text_buffer;
}
