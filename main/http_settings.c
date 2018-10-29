#include "http_settings.h"
#include "config_parser.h"
#include "wifi_task.h"

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <esp_ota_ops.h>
#include <sys/time.h>
#include <time.h>


static const char *TAG="WEB";

#define MAX_BUFFER_SIZE  2048

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
/*
static const char welcome[] = "<!DOCTYPE html><html><body>"
                              "<form method='POST' action='/config'>"
                              "<br><p>Wifi:</p><br>"
                              "ssid:<input type='text' name='ssid' maxlength='32' value='%s'><br>"
                              "password:<input type='password' name='psk' value='********' maxlength='32'><br>"
                              "<br><p>Time:</p><br>"
                              "%s<br>"
                              "Date:<input type='date' name='set_date'><br>"
                              "Time:<input type='time' name='set_time'><br>"
                              "<input type='submit' value='Update'></form>"
                              "</body></html>";
*/
/* Our URI handler function to be called during GET /uri request */
static esp_err_t get_handler(httpd_req_t *req)
{
    if ( !strcmp(req->uri, "/") ||
         !strcmp(req->uri, "/index.html") )
    {
        char *resp = malloc( MAX_BUFFER_SIZE );
        time_t t = time( NULL );
        snprintf( resp, MAX_BUFFER_SIZE, index_html_start, app_wifi_get_sta_ssid(),
                  ctime( &t ) );
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_send(req, resp, strlen(resp));
        free(resp);
    }
    else
    {
        httpd_resp_set_status(req, "404 Not found");
        httpd_resp_send(req, req->uri, strlen(req->uri));
    }
    return ESP_OK;
}

extern int httpd_recv(httpd_req_t *r, char *buf, size_t buf_len);

/* Our URI handler function to be called during POST /uri request */
static esp_err_t upload_config(httpd_req_t *req)
{
    /* Read request content */
    char content[128];

    /* Truncate if content length larger than the buffer */
    size_t total_size = req->content_len;

    while (total_size > 0)
    {
        size_t recv_size = sizeof(content)-1;
//        int ret = httpd_recv(/*httpd_req_recv(*/req, content, recv_size);
        int ret = httpd_req_recv(req, content, recv_size);
        if (ret < 0)
        {
            ESP_LOGE(TAG, "failed to read data");
            /* In case of recv error, returning ESP_FAIL will
             * ensure that the underlying socket is closed */
            return ESP_FAIL;
        }
        total_size -= ret;
        content[ret] = '\0';
        ESP_LOGD(TAG, "%s\n", content);
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, content, ret, ESP_LOG_INFO);
    }
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    if ( apply_new_config(content, req->content_len) < 0 )
    {
        const char resp[]="Failed to apply changes";
        httpd_resp_set_status(req, HTTPD_404);
        httpd_resp_send(req, resp, sizeof(resp));
    }
    else
    {
        const char resp[]="Applied"
                          "<script type=\"text/JavaScript\">"
                          "setTimeout(\"location.href = '/';\",2000);"
                          "</script>";
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_send(req, resp, sizeof(resp));
    }

    return ESP_OK;
}

/* Our URI handler function to be called during POST /uri request */
static esp_err_t fw_update_callback(httpd_req_t *req)
{
    /* Read request content */
    char content[128];

    /* Truncate if content length larger than the buffer */
    size_t total_size = req->content_len;

    while (total_size > 0)
    {
        size_t recv_size = sizeof(content);
//        int ret = httpd_recv(/*httpd_req_recv(*/req, content, recv_size);
        int ret = httpd_req_recv(req, content, recv_size);
        if (ret < 0)
        {
            ESP_LOGE(TAG, "failed to read data");
            /* In case of recv error, returning ESP_FAIL will
             * ensure that the underlying socket is closed */
            return ESP_FAIL;
        }
        total_size -= ret;
        content[ret] = '\0';
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, content, ret, ESP_LOG_INFO);
    }
    return ESP_OK;
}


/* URI handler structure for GET /uri */
static httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

/* URI handler structure for POST /uri */
static httpd_uri_t uri_config = {
    .uri      = "/config",
    .method   = HTTP_POST,
    .handler  = upload_config,
    .user_ctx = NULL
};

static httpd_uri_t uri_update = {
    .uri      = "/fwupdate",
    .method   = HTTP_POST,
    .handler  = fw_update_callback,
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

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_config);
        httpd_register_uri_handler(server, &uri_update);
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

