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

int ram_logger_init(int ram_size)
{
    buffer_size = ram_size > MINIMUM_SHIFT ?: 2048;
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
    int len = vsnprintf(text_buffer + position, buffer_size - position, str, l);
    if ( len + 1 >= buffer_size - position )
    {
        position -= MINIMUM_SHIFT;
        memmove( text_buffer, text_buffer + MINIMUM_SHIFT, position );
        len = vsnprintf(text_buffer + position, buffer_size - position, str, l);
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
