#include "http_ota_server.h"

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <http_server.h>
#include <esp_ota_ops.h>


/* Our URI handler function to be called during GET /uri request */
static esp_err_t get_handler(httpd_req_t *req)
{
    if ( !strcmp(req->uri, "/") ||
         !strcmp(req->uri, "/index.html") )
    {
        const char resp[] = "<form method='POST' action='/update' "
                            "enctype='multipart/form-data'><input type='file' name='update'>"
                            "<input type='submit' value='Update'></form>";
        httpd_resp_set_status(req, "200 OK");
        httpd_resp_send(req, resp, strlen(resp));
    }
    else
    {
        httpd_resp_set_status(req, "404 Not found");
    }
    return ESP_OK;
}

/* Our URI handler function to be called during POST /uri request */
static esp_err_t upload_fw_handler(httpd_req_t *req)
{
    /* Read request content */
    char content[128];

    /* Truncate if content length larger than the buffer */
    size_t total_size = req->content_len;

    // init flash write
    const esp_partition_t* active_partition = esp_ota_get_running_partition();
    const esp_partition_t* next_partition = esp_ota_get_next_update_partition(active_partition);
    if (next_partition)
    {
        httpd_resp_set_status(req, "404 Invalid partition");
        ESP_LOGE("web", "failed to prepare partition");
        return ESP_OK;
    }
    esp_ota_handle_t ota_handle = 0;
/*    esp_err_t err = esp_ota_begin(next_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK)
    {
        httpd_resp_set_status(req, "404 Invalid partition");
        ESP_LOGE("web", "failed to prepare partition 2");
        return ESP_OK;
    } */
    while (total_size > 0)
    {
        size_t recv_size = MIN(total_size, sizeof(content));
        int ret = httpd_req_recv(req, content, recv_size);
        if (ret < 0)
        {
            ESP_LOGE("web", "failed to read data");
            /* In case of recv error, returning ESP_FAIL will
             * ensure that the underlying socket is closed */
            return ESP_FAIL;
        }
//        printf("%s\n", content);
        ESP_LOG_BUFFER_HEX_LEVEL("web", content, ret, ESP_LOG_INFO);
//        esp_ota_write(ota_handle, content, ret);
        total_size -= ret;
        // write to flash
    }
//    esp_ota_end(ota_handle);

    /* Send a simple response */
    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, strlen(resp));

    // complete flash write
    esp_ota_set_boot_partition(next_partition);
    esp_restart();
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
static httpd_uri_t uri_upload_fw = {
    .uri      = "/update",
    .method   = HTTP_POST,
    .handler  = upload_fw_handler,
    .user_ctx = NULL
};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Empty handle to http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_upload_fw);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}

