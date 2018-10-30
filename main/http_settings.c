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
static esp_err_t main_index_handler(httpd_req_t *req)
{
    if ( !strcmp(req->uri, "/") ||
         !strcmp(req->uri, "/index.html") )
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
        httpd_resp_send(req, index_html_start, strlen(index_html_start));
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
    char content[64];
//    esp_err_t err = httpd_req_get_hdr_value_str(req, "name", content, sizeof(content));
    esp_err_t err = httpd_req_get_url_query_str(req, content, sizeof(content));
    if (err == ESP_OK )
    {
        char val[64];
        ESP_LOGD(TAG, "content: %s", content);
        err = httpd_query_key_value( content, "name", val, sizeof(val));
        if ( err == ESP_OK )
        {
            if ( get_config_value( val, val, sizeof(val)) != 0 )
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
        httpd_resp_send(req, req->uri, strlen(req->uri));
    }
    return ESP_OK;
}

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

static const char UPGRADE_ERR_PARTITION_NOT_FOUND[] = "Failed to detect partition for upgrade\n";
static const char UPGRADE_ERR_FAILED_TO_START[] = "Failed to start OTA\n";
static const char UPGRADE_ERR_FAILED_TO_WRITE[] = "Failed to write partition\n";
static const char UPGRADE_ERR_VERIFICATION_FAILED[] = "Invalid firmware detected\n";

/* Our URI handler function to be called during POST /uri request */
static esp_err_t fw_update_callback(httpd_req_t *req)
{
    /* Read request content */
    char content[128];
    const char* error_msg = NULL;

    esp_ota_handle_t ota_handle = 0;
    const esp_partition_t* next_partition = NULL;

    /* Truncate if content length larger than the buffer */
    size_t total_size = req->content_len;

    const esp_partition_t* active_partition = esp_ota_get_running_partition();
    next_partition = esp_ota_get_next_update_partition(active_partition);
    if (!next_partition)
    {
        error_msg = UPGRADE_ERR_PARTITION_NOT_FOUND;
        goto error;
    }
    esp_err_t err = esp_ota_begin(next_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK)
    {
        error_msg = UPGRADE_ERR_FAILED_TO_START;
        goto error;
    }

    while (total_size > 0)
    {
        size_t recv_size = sizeof(content);
        int ret = httpd_req_recv(req, content, recv_size);
        if (ret < 0)
        {
            /* In case of recv error, returning ESP_FAIL will
             * ensure that the underlying socket is closed */
            esp_ota_end(ota_handle);
            return ESP_FAIL;
        }
        total_size -= ret;
        if ( esp_ota_write(ota_handle, content, ret) != ESP_OK)
        {
            error_msg = UPGRADE_ERR_FAILED_TO_WRITE;
            goto error;
        }
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, content, ret, ESP_LOG_DEBUG);
    }
    if ( esp_ota_end(ota_handle) != ESP_OK )
    {
        error_msg = UPGRADE_ERR_VERIFICATION_FAILED;
        goto error;
    }
    const char resp[]="SUCCESS";
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_send(req, resp, sizeof(resp));
    ESP_LOGI(TAG, "Upgrade successful");
    esp_ota_set_boot_partition(next_partition);
    ota_handle = 0;
    fflush(stdout);
    /** Delay before reboot */
    vTaskDelay( 2000 / portTICK_PERIOD_MS );
    esp_restart();
    /* We never go to this place */
    return ESP_OK;    
error:
    if (!ota_handle)
    {
        esp_ota_end(ota_handle);
    }
    esp_log_write( ESP_LOG_ERROR, TAG, error_msg );
    httpd_resp_set_status(req, HTTPD_400);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, error_msg, strlen(error_msg));
    return ESP_OK;
}

static httpd_uri_t uri_index = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = main_index_handler,
    .user_ctx = NULL
};

static httpd_uri_t uri_param = {
    .uri      = "/param",
    .method   = HTTP_GET,
    .handler  = param_handler,
    .user_ctx = NULL
};

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
        httpd_register_uri_handler(server, &uri_index);
        httpd_register_uri_handler(server, &uri_param);
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

