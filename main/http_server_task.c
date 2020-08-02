#include "http_server_task.h"
#include "http_applet_engine.h"
#include "utils.h"
#include "http_ota_upgrade.h"
#include "wifi_task.h"
#include "clock_events.h"
#include "clock_settings.h"
#include "ram_logger.h"
#include "platform/system.h"
#include "http_digest.h"
#include "http_form_auth.h"
#include "clock_settings.h"

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

// Uncomment this if SSL server support is required
#define USE_SSL_WEB_SERVER

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

static uint8_t validate_session(httpd_req_t *req, const char *method)
{
    char *session = malloc(512);
    // First check token in cookie header
    if ( httpd_req_get_hdr_value_str( req, "Cookie", session, 512 ) == ESP_OK )
    {
        //ESP_LOGI( TAG, "Cookie: %s", session );
        char *token_value = strstr( session, "token=" );
        if ( token_value != NULL )
        {
            char *endp = strchr( token_value, ';' );
            if (endp) *endp='\0';
            if ( form_auth_server_validate_token( token_value + strlen("token="), settings_get_user(), settings_get_pass() ) )
            {
                free( session );
                return 1;
            }
        }
    }
    // If token is not valid, check Digest authorization header
    esp_err_t err;
    if ( (err = httpd_req_get_hdr_value_str( req, "Authorization", session, 512 )) == ESP_OK )
    {
        if ( digest_server_validate_session_creds( session, method, "NixieClock", settings_get_user(), settings_get_pass() ) )
        {
            free( session );
            return 1;
        }
    }
    if ( err == ESP_ERR_HTTPD_RESULT_TRUNC )
    {
    }
    free( session );
    ESP_LOGI(TAG, "NOT Authorized HTTP request!!!");
    return 0;
}

static void redirect_to_login_page(httpd_req_t *req, bool digest)
{
    // Some resources can be accessed using Digest authorization
    // While other requires more beautiful method, based on tokens
    if ( !digest )
    {
        httpd_resp_set_status(req, "307 Temporary redirect");
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }
    else
    {
        httpd_resp_set_status(req, "401 Unauthorized");
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        char p[256];
        digest_server_www_auth_request(p, 256, "NixieClock");
        httpd_resp_set_hdr(req, "WWW-Authenticate", p);
        httpd_resp_send(req, NULL, 0);
    }
}

/* Our URI handler function to be called during GET /uri request */
static esp_err_t main_index_handler(httpd_req_t *req)
{
    uint8_t authenticated = validate_session(req, "GET");
    if ( !authenticated && strcmp(req->uri, "/login.html") && strcmp(req->uri, "/styles.css") && strcmp(req->uri, "/favicon.ico") )
    {
        redirect_to_login_page(req, false);
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
    uint8_t authenticated = validate_session(req, "POST");
    if ( !authenticated)
    {
        redirect_to_login_page(req, false);
        return false;
    }
    return true;
}

static esp_err_t param_handler(httpd_req_t *req)
{
    uint8_t authenticated = validate_session(req, "POST");
    if ( !authenticated)
    {
        redirect_to_login_page(req, true);
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
    // Login update always requires authorization
    if ( !strcmp(req->uri, "/login_update") && !validate_session(req, "POST") )
    {
        redirect_to_login_page(req, false);
        return ESP_OK;
    }
    esp_err_t err = ESP_FAIL;
    /* Truncate if content length larger than the buffer */
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret >= 0)
    {
        content[ret] = '\0';
        decode_url_in_place(content);
        int size = sizeof(token) - strlen(token);
        char * user = credentials;
        if ( httpd_query_key_value( content, "uname", user, size) == ESP_OK )
        {

            size -= (strlen(user) + 1);
            char * password = user + strlen(user) + 1;
            if ( httpd_query_key_value( content, "psw", password, size) == ESP_OK )
            {
                // If login update is requested, then change login settings,
                if ( !strcmp(req->uri, "/login_update") )
                {
                    load_settings();
                    settings_set_user( user );
                    settings_set_pass( password );
                    save_settings();
                    err = ESP_OK;
                }
                else
                {
                    form_auth_server_generate_token( credentials, sizeof(token) - (credentials - token), user, password );
                    ESP_LOGI( TAG, "Generated token: '%s'", token );
                    if ( form_auth_server_validate_token( credentials, settings_get_user(), settings_get_pass() ) )
                    {
                        strcat( token, "; path=/" );
                        err = ESP_OK;
                    }
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

static httpd_uri_t uri_index_html = {
    .uri      = "/index.html",
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

static httpd_uri_t uri_auth_update = {
    .uri      = "/login_update",
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



#if defined(USE_SSL_WEB_SERVER)
static httpd_handle_t server = NULL;
extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
#else
static httpd_handle_t server = NULL;
#endif

/* Function for starting the webserver */
void start_webserver(void)
{
    if (server != NULL)
    {
        ESP_LOGI(TAG, "Http server is already started");
        return;
    }
    /* Generate default configuration */
#if defined(USE_SSL_WEB_SERVER)
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    config.cacert_pem = cacert_pem_start;
    config.cacert_len = cacert_pem_end - cacert_pem_start;
    config.prvtkey_pem = prvtkey_pem_start;
    config.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
    config.httpd.task_priority = 4;
    config.httpd.recv_wait_timeout = 10;
    config.httpd.max_uri_handlers = 12;

    /* Start the httpd server */
    if (httpd_ssl_start(&server, &config) == ESP_OK)
#else
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.task_priority = 4;
    config.recv_wait_timeout = 10;
    config.max_uri_handlers = 12;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
#endif
    {
        applet_engine_init( &engine, NULL );
        applet_engine_set_params( &engine, config_params );
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_index);
        httpd_register_uri_handler(server, &uri_index_html);
        register_httpd_ota_handler(server, before_upgrade_check, on_upgrade_start, on_upgrade_end );
        httpd_register_uri_handler(server, &uri_debug);
        httpd_register_uri_handler(server, &uri_param);
        httpd_register_uri_handler(server, &uri_login);
        httpd_register_uri_handler(server, &uri_auth_update);
        httpd_register_uri_handler(server, &uri_auth);
        httpd_register_uri_handler(server, &uri_styles);
        httpd_register_uri_handler(server, &uri_favicon);
        if ( httpd_register_uri_handler(server, &uri_log) != ESP_OK )
        {
            // Fallback to previous release
            send_app_event( EVT_APP_STOP, 0 );
        }
        ESP_LOGI(TAG, "server is started");
    }
    else
    {
        // Fallback to previous release
        send_app_event( EVT_APP_STOP, 0 );
    }
}

/* Function for stopping the webserver */
void stop_webserver(void)
{
    if (server)
    {
        /* Stop the httpd server */
#if defined(USE_SSL_WEB_SERVER)
        httpd_ssl_stop(server);
#else
        httpd_stop(server);
#endif
        applet_engine_close( &engine );
        server = NULL;
        ESP_LOGI(TAG, "server is stopped");
    }
}

