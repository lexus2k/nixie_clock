#include "tftp_task.h"
#include "tftp_ota_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


TftpOtaServer server;

static void tftp_task(void *pvParameters)
{
    while (server.run()>=0)
    {
    }
    vTaskDelete( NULL );
}


void start_tftp(void)
{
    if (server.start()<0)
    {
        return;
    }
    xTaskCreate(&tftp_task, "tftp_task", 4096, NULL, 5, NULL);
}

void stop_tftp(void)
{
    server.stop();
}
