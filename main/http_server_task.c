#include "http_server_task.h"
#include "http_applet_engine.h"
#include "utils.h"
#include "http_ota_upgrade.h"
#include "wifi_task.h"
#include "clock_events.h"
#include "clock_settings.h"
#include "ram_logger.h"
#include "platform/system.h"

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <mbedtls/base64.h>
#include <mbedtls/md.h>

#include <esp_ota_ops.h>
#include <ctype.h>
#include <stdio.h>


static const char *TAG="WEB";
static const uint32_t MAX_BLOCK_SIZE = 1536;

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
extern const char login_html_start[] asm("_binary_login_html_start");
extern const char login_html_end[]   asm("_binary_login_html_end");
extern const char debug_html_start[] asm("_binary_debug_html_start");
extern const char debug_html_end[]   asm("_binary_debug_html_end");
extern const char styles_css_start[] asm("_binary_styles_css_start");
extern const char styles_css_end[]   asm("_binary_styles_css_end");
extern const char favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const char favicon_ico_end[]   asm("_binary_favicon_ico_end");

applet_engine_t engine;
static uint32_t s_salt = 0;
static uint64_t s_last_access_ts = 0;

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

static void generate_html_content( httpd_req_t *req, const char *basepage )
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
        applet_engine_set_html( &engine, basepage, strlen(basepage) );
        do
        {
            len = applet_engine_process( &engine, content, MAX_BLOCK_SIZE );
            if ( httpd_resp_send_chunk(req, len ? content: NULL, len) != ESP_OK )
            {
                break;
            }
        } while (len != 0);
        free( content );
    }
}

// 1 if session is valid, otherwise 0
static int generate_token(char *token, int max_len, const char *data)
{
    char hash[MBEDTLS_MD_MAX_SIZE];
    const mbedtls_md_info_t *md_info4 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (md_info4 == NULL)
    {
        ESP_LOGE(TAG, "Failed to run sha256 algo");
        return 0;
    }
    mbedtls_md_context_t ctx4;
    mbedtls_md_init(&ctx4);
//    int ret4 = mbedtls_md_init_ctx(&ctx4, md_info4);
    int ret4 = mbedtls_md_setup(&ctx4, md_info4, 0);
    if (ret4 != 0)
    {
        ESP_LOGE(TAG, "Failed to run sha256 algo");
        return 0;
    }
    if ( (uint64_t)(micros64() - s_last_access_ts) > 5*60000000 ) // 5 minutes timeout for web access
    {
        s_salt++;
    }
    s_last_access_ts = micros64();
    mbedtls_md_starts(&ctx4);
    mbedtls_md_update(&ctx4, (const unsigned char *)&s_salt, sizeof(s_salt));
    mbedtls_md_update(&ctx4, (const unsigned char *)data, strlen(data));
    mbedtls_md_finish(&ctx4, (unsigned char *)hash);
    mbedtls_md_free(&ctx4);
    size_t len;
    mbedtls_base64_encode( (unsigned char *)token, max_len, &len, (const unsigned char *)hash, mbedtls_md_get_size(md_info4));
    return len;
}

static uint8_t validate_token(const char *token)
{
    char hash[MBEDTLS_MD_MAX_SIZE*2] = { 0 };
    int len = generate_token( hash, sizeof(hash), "admin:password" );
    if (len && !strncmp(token, hash, len))
    {
        return 1;
    }
    return 0;
}

static uint8_t validate_session(httpd_req_t *req)
{
    char session[MBEDTLS_MD_MAX_SIZE*2] = { 0 };
    if ( httpd_req_get_hdr_value_str( req, "Cookie", session, sizeof(session) ) == ESP_OK )
    {
        //ESP_LOGI( TAG, "Cookie: %s", session );
        char *token_value = strstr( session, "token=" );
        if ( token_value != NULL )
        {
            char *endp = strchr( token_value, ';' );
            if (endp) *endp='\0';
            if ( validate_token( token_value + strlen("token=") ) )
            {
                return 1;
            }
        }
    }
    ESP_LOGI(TAG, "NOT Authorized HTTP request!!!");
    return 0;
}

static void redirect_to_login_page(httpd_req_t *req)
{
    httpd_resp_set_status(req, "301 Moved Permanently");
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_set_hdr(req, "Location", "/login.html");
    httpd_resp_send(req, NULL, 0);
}

/* Our URI handler function to be called during GET /uri request */
static esp_err_t main_index_handler(httpd_req_t *req)
{
    uint8_t authenticated = validate_session(req);
    if ( !authenticated && strcmp(req->uri, "/login.html") && strcmp(req->uri, "/styles.css") && strcmp(req->uri, "/favicon.ico") )
    {
        redirect_to_login_page(req);
    }
    else if ( !strcmp(req->uri, "/login.html") )
    {
        generate_html_content( req, login_html_start );
    }
    else if ( !strcmp(req->uri, "/") ||
         !strcmp(req->uri, "/index.html") )
    {
        generate_html_content( req, index_html_start );
    }
    else if ( !strcmp(req->uri, "/debug.html") )
    {
        generate_html_content( req, debug_html_start );
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

static bool before_upgrade_check(httpd_req_t *req)
{
    uint8_t authenticated = validate_session(req);
    if ( !authenticated)
    {
        redirect_to_login_page(req);
        return false;
    }
    return true;
}

static esp_err_t param_handler(httpd_req_t *req)
{
    uint8_t authenticated = validate_session(req);
    if ( !authenticated)
    {
        redirect_to_login_page(req);
        return ESP_OK;
    }
    /* Read request content */
    char content[128];
    esp_err_t err = ESP_FAIL;
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
                applet_engine_write_var( &engine, name, val, sizeof(val) );
                httpd_resp_set_status(req, HTTPD_200);
                httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
                httpd_resp_send(req, val, strlen(val));
            }
            else if ( applet_engine_read_var( &engine, name, val, sizeof(val)) < 0 )
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

static esp_err_t login_handler(httpd_req_t *req)
{
    /* Read request content */
    char content[128];
    char token[MBEDTLS_MD_MAX_SIZE*2] = "token=";
    char *credentials = token + strlen(token);
    esp_err_t err = ESP_FAIL;
    /* Truncate if content length larger than the buffer */
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret >= 0)
    {
        content[ret] = '\0';
        decode_url_in_place(content);
        int size = sizeof(token) - strlen(token);
        char * p = credentials;
        if ( httpd_query_key_value( content, "uname", p, size) == ESP_OK )
        {
            strcat( token, ":" );
            size -= strlen(p);
            p += strlen(p);
            if ( httpd_query_key_value( content, "psw", p, size) == ESP_OK )
            {
                generate_token( credentials, sizeof(token) - (credentials - token), credentials );
                ESP_LOGI( TAG, "Generated token: '%s'", token );
                if ( validate_token( credentials ) )
                {
                    strcat( token, "; path=/" );
                    err = ESP_OK;
                }
            }
        }
    }
    if (err != ESP_OK)
    {
        httpd_resp_set_status(req, "401 Not authorized");
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_send(req, "<html><body>Not authorized</body></html>", 0);
    }
    else
    {
        // redirect to main page
        httpd_resp_set_status(req, "301 Moved Permanently");
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_set_hdr(req, "Location", "/");
        httpd_resp_set_hdr(req, "Set-Cookie:", token);
        httpd_resp_send(req, NULL, 0);
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

static httpd_uri_t uri_login = {
    .uri      = "/login.html",
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

static httpd_uri_t uri_auth = {
    .uri      = "/login",
    .method   = HTTP_POST,
    .handler  = login_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_log = {
    .uri      = "/log",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
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
    config.recv_wait_timeout = 10;
    config.max_uri_handlers = 12;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        applet_engine_init( &engine, NULL );
        applet_engine_set_params( &engine, config_params );
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_index);
        register_httpd_ota_handler(server, before_upgrade_check, on_upgrade_start, on_upgrade_end );
        httpd_register_uri_handler(server, &uri_debug);
        httpd_register_uri_handler(server, &uri_param);
        httpd_register_uri_handler(server, &uri_login);
        httpd_register_uri_handler(server, &uri_auth);
        httpd_register_uri_handler(server, &uri_styles);
        httpd_register_uri_handler(server, &uri_favicon);
        httpd_register_uri_handler(server, &uri_log);
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

