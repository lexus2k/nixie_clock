#include "http_server_task.h"
#include "http_applet_engine.h"
#include "states/utils.h"
#include "http_ota_upgrade.h"
#include "wifi_task.h"
#include "clock_events.h"
#include "clock_settings.h"
#include "ram_logger.h"

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

applet_engine_t engine;

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

//void applet_engine_set_html(applet_engine_t *engine, const char *html, int len);
//int applet_engine_process(applet_engine_t *engine, char *buffer, int buffer_size);

/* Our URI handler function to be called during GET /uri request */
static esp_err_t main_index_handler(httpd_req_t *req)
{
    if ( !strcmp(req->uri, "/") ||
         !strcmp(req->uri, "/index.html") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
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
            applet_engine_start( index_html_start, strlen(index_html_start) );
            do
            {
                len = applet_engine_get_chunk( content, MAX_BLOCK_SIZE, &get_config_value );
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
            applet_engine_start( debug_html_start, strlen(debug_html_start) );
            do
            {
                len = applet_engine_get_chunk( content, MAX_BLOCK_SIZE, &get_config_value);
                if ( httpd_resp_send_chunk(req, len ? content: NULL, len) != ESP_OK )
                {
                    break;
                }
            } while (len != 0);
            free( content );
        }
    }
    else if ( !strcmp(req->uri, "/log") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_send(req, ram_logger_get_logs(), -1);
    }
    else if ( !strcmp(req->uri, "/styles.css") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, "text/css");
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
                try_config_value( name, val, sizeof(val) );
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

static httpd_uri_t uri_log = {
    .uri      = "/log",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
    .user_ctx = NULL
};

static httpd_handle_t server = NULL;

int applet_callback_default( applet_callback_data_t *data, void *user_data )
{
    if ( data->write ) return -1;
    return get_config_value( data->name, data->r.data, data->r.max_len );
}

applet_param_t apt_params[] = {
    { NULL, &applet_callback_default },
    { NULL, NULL },
};

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
    config.recv_wait_timeout = 10;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        applet_engine_init( &engine, NULL );
        applet_engine_set_params( &engine, apt_params );
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_index);
        httpd_register_uri_handler(server, &uri_debug);
        httpd_register_uri_handler(server, &uri_styles);
        httpd_register_uri_handler(server, &uri_favicon);
        httpd_register_uri_handler(server, &uri_param);
        httpd_register_uri_handler(server, &uri_log);
        register_httpd_ota_handler( server, on_upgrade_start, on_upgrade_end );
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
        applet_engine_close( &engine );
        server = NULL;
        ESP_LOGI(TAG, "server is stopped");
    }
}

