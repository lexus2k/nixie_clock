//#include "nixie_clock.h"
#include "utils.h"
#include "clock_display.h"
#include "clock_hardware.h"
#include "clock_buttons.h"
#include "clock_time.h"
#include "states/clock_states.h"
#include "clock_events.h"

//#include "http_server_task.h"
//#include "wifi_task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "mdns.h"
#include "version.h"

#include "http_ota_upgrade.h"

static const char* TAG = "UTILS";

bool start_mdns_service(void)
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init mDNS");
        return false;
    }
    mdns_hostname_set("nixie-clock");
    mdns_instance_name_set("Nixie ESP32 clock");
    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    ESP_LOGI(TAG, "mDNS initialized");
    return true;
}

void stop_mdns_service(void)
{
    mdns_service_remove_all();
    vTaskDelay(1);
    mdns_free();
    ESP_LOGI(TAG, "mDNS stopped");
}

bool on_validate_version(const char * new_ver)
{
    ESP_LOGI(TAG, "CURRENT FW: %s", FW_VERSION);
    ESP_LOGI(TAG, "NEW FW DETECTED: %s", new_ver);
    return is_version_newer(new_ver);
}

void on_upgrade_start(void)
{
    send_app_event( EVT_UPGRADE_STATUS, EVT_UPGRADE_STARTED );
}

void on_upgrade_end(bool success)
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

void load_hardware_configuration()
{
    gpio_iomux_out(GPIO_NUM_12, FUNC_MTDI_GPIO12, false);
    gpio_iomux_out(GPIO_NUM_34, FUNC_GPIO34_GPIO34, false);
    gpio_iomux_out(GPIO_NUM_17, FUNC_GPIO17_GPIO17, false);
    gpio_iomux_out(GPIO_NUM_35, FUNC_GPIO35_GPIO35, false);

    if ( !strncmp(settings.factory().get_serial_number(), "IN14", 4) )
    {
        display.setup_in14();
    }
    else if ( !strncmp(settings.factory().get_serial_number(), "IN12", 4) )
    {
        display.setup_in12a();
    }

    if (settings.factory().get_revision() == 1)
    {
        abuttons.setup( ADC1_CHANNEL_0, { 640, 479, 298 } );
        dbuttons.setup( { { GPIO_NUM_0, 0 }, } );
    }
    else if (settings.factory().get_revision() == 2)
    {
        gpio_iomux_out(GPIO_NUM_36, FUNC_GPIO36_GPIO36, false);
        gpio_set_direction(GPIO_NUM_36, GPIO_MODE_INPUT);
        abuttons.setup( ADC1_CHANNEL_6, { 640, 479, 298 } );
        dbuttons.setup( { { GPIO_NUM_0, 0 }, { GPIO_NUM_4, 0 } } );
        temperature.setup( ADC1_CHANNEL_3, ADC_WIDTH_BIT_10 );
        als.setup( ADC1_CHANNEL_0, ADC_WIDTH_BIT_10 );
        als.setup_peak_detector( 30 );
    }
}

