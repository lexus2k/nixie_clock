#include "http_server_task.h"
#include "http_ota_upgrade.h"
#include "wifi_task.h"
#include "clock_events.h"
#include "clock_settings.h"

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <esp_ota_ops.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>


static const char *TAG="WEB";
static const uint32_t MAX_BLOCK_SIZE = 1536;

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
extern const char debug_html_start[] asm("_binary_debug_html_start");
extern const char debug_html_end[]   asm("_binary_debug_html_end");
extern const char styles_css_start[] asm("_binary_styles_css_start");
extern const char styles_css_end[]   asm("_binary_styles_css_end");
extern const char favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const char favicon_ico_end[]   asm("_binary_favicon_ico_end");

static unsigned int hex_char_to_uint(char hex)
{
    if (hex <= '9') return hex - '0';
    if (hex <='F') return hex - 'A' + 10;
    return hex - 'a' + 10;
}

static void decode_url_in_place(char *str)
{
    while (*str)
    {
        if (*str == '%' && str[1]!='\0' && str[2] != '\0')
        {
            unsigned char a = str[1];
            unsigned char b = str[2];
            if (isxdigit(a) && isxdigit(b))
            {
                *str = hex_char_to_uint(a) * 16 + hex_char_to_uint(b);
                memmove(str+1, str+3, strlen(str+2));
            }
        }
        str++;
    }
}

static int get_next_chunk_block(char *block, int max_len,
                                const char **src, const  char *end,
                                int (*applet_cb)(const char *applet, char *result, int max_len))
{
    const int min_size = 64;
    int len = 0;
    bool call_found = false;
    while (*src < end && len < max_len)
    {
        if ( (*src)[0] == '[' && *src + 1 < end && (*src)[1] == '%')
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
            (*src) += 2;
            const char *p = strchr( *src, '%' );
            if (!p)
            {
                ESP_LOGE(TAG, "Failed to parse web page at: %s", *src);
            }
            else
            {
                strncpy( block, *src, p - *src );
                block[p - *src] = '\0';
                *src = p + 2;
                if (applet_cb( block, block, max_len - len ) != 0)
                {
                    ESP_LOGE(TAG, "Failed to get value for: %s", block);
                }
                else
                {
                    len += strlen(block);
                    block += strlen(block);
                }
            }
            call_found = false;
        }
        else
        {
            *block = **src;
            (*src)++;
            len++;
            block++;
        }
    }
    return len;
}

/* Our URI handler function to be called during GET /uri request */
static esp_err_t main_index_handler(httpd_req_t *req)
{
    if ( !strcmp(req->uri, "/") ||
         !strcmp(req->uri, "/index.html") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        const char *p = index_html_start;
        const char *end = index_html_start + strlen(index_html_start);
        char *content = malloc( MAX_BLOCK_SIZE );
        if ( !content )
        {
            httpd_resp_set_status(req, HTTPD_400);
            httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
            httpd_resp_send(req, "Internal problem", -1);
        }
        else
        {
            int len;
            do
            {
                len = get_next_chunk_block( content, MAX_BLOCK_SIZE, &p, end, &get_config_value);
                if ( httpd_resp_send_chunk(req, len ? content: NULL, len) != ESP_OK )
                {
                    break;
                }
            } while (len != 0);
            free( content );
        }
    }
    else if ( !strcmp(req->uri, "/debug.html") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        const char *p = debug_html_start;
        const char *end = debug_html_start + strlen(debug_html_start);
        char *content = malloc( MAX_BLOCK_SIZE );
        if ( !content )
        {
            httpd_resp_set_status(req, HTTPD_400);
            httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
            httpd_resp_send(req, "Internal problem", -1);
        }
        else
        {
            int len;
            do
            {
                len = get_next_chunk_block( content, MAX_BLOCK_SIZE, &p, end, &get_config_value);
                if ( httpd_resp_send_chunk(req, len ? content: NULL, len) != ESP_OK )
                {
                    break;
                }
            } while (len != 0);
            free( content );
        }
    }
    else if ( !strcmp(req->uri, "/styles.css") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_send(req, styles_css_start, strlen(styles_css_start));
    }
    else if ( !strcmp(req->uri, "/favicon.ico") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, "image/x-icon");
        httpd_resp_send(req, favicon_ico_start, favicon_ico_end - favicon_ico_start);
    }
    else
    {
        httpd_resp_set_status(req, "404 Not found");
        httpd_resp_send(req, req->uri, strlen(req->uri));
    }
    return ESP_OK;
}

static esp_err_t param_handler(httpd_req_t *req)
{
    /* Read request content */
    char content[128];
    esp_err_t err = ESP_FAIL;
//    esp_err_t err = httpd_req_get_url_query_str(req, content, sizeof(content));
//    if (err == ESP_OK )

    /* Truncate if content length larger than the buffer */
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret >= 0)
    {
        content[ret] = '\0';
        char name[16];
        ESP_LOGD(TAG, "content: %s", content);
        decode_url_in_place(content);
        err = httpd_query_key_value( content, "name", name, sizeof(name));
        if ( err == ESP_OK )
        {
            char val[64];
            if ( httpd_query_key_value( content, "value", val, sizeof(val)) == ESP_OK )
            {
                if (!strcmp(name, "apply") && !strcmp(val,"true"))
                {
                    save_settings();
                }
                else if (!strcmp(name, "apply_wifi") && !strcmp(val,"true"))
                {
                    app_wifi_apply_sta_settings();
                }
                else
                {
                    try_config_value( name, val, sizeof(val) );
                }
                httpd_resp_set_status(req, HTTPD_200);
                httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
                httpd_resp_send(req, val, strlen(val));
            }
            else if ( get_config_value( name, val, sizeof(val)) != 0 )
            {
                err = ESP_FAIL;
            }
            else
            {
                httpd_resp_set_status(req, HTTPD_200);
                httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
                httpd_resp_send(req, val, strlen(val));
            }
        }
    }
    if (err != ESP_OK)
    {
        httpd_resp_set_status(req, "404 Not found");
        if (ret >= 0)
        {
            httpd_resp_send(req, content, strlen(content));
        }
        else
        {
            httpd_resp_send(req, req->uri, strlen(req->uri));
        }
    }
    return ESP_OK;
}

static void on_upgrade_start(void)
{
    send_app_event( EVT_UPGRADE_STATUS, EVT_UPGRADE_STARTED );
}

static void on_upgrade_end(bool success)
{
    if ( success )
    {
        send_app_event( EVT_UPGRADE_STATUS, EVT_UPGRADE_SUCCESS );
    }
    else
    {
        send_app_event( EVT_UPGRADE_STATUS, EVT_UPGRADE_FAILED );
    }
}

static httpd_uri_t uri_index = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_debug = {
    .uri      = "/debug.html",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_styles = {
    .uri      = "/styles.css",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_favicon = {
    .uri      = "/favicon.ico",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_param = {
    .uri      = "/param",
    .method   = HTTP_POST,
    .handler  = param_handler,
    .user_ctx = NULL
};

static httpd_handle_t server = NULL;

/* Function for starting the webserver */
void start_webserver(void)
{
    if (server != NULL)
    {
        ESP_LOGI(TAG, "Http server is already started");
        return;
    }
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.task_priority = 4;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_index);
        httpd_register_uri_handler(server, &uri_debug);
        httpd_register_uri_handler(server, &uri_styles);
        httpd_register_uri_handler(server, &uri_favicon);
        httpd_register_uri_handler(server, &uri_param);
        register_ota_handler( server, on_upgrade_start, on_upgrade_end );
        ESP_LOGI(TAG, "server is started");
    }
}

/* Function for stopping the webserver */
void stop_webserver(void)
{
    if (server)
    {
        /* Stop the httpd server */
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "server is stopped");
    }
}

