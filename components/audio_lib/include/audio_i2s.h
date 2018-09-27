#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_spi_flash.h"
//#include "esp_err.h"
//#include "esp_log.h"
//#include "esp_partition.h"
#include "driver/i2s.h"
#include "driver/adc.h"
//#include "audio_example_file.h"
#include "esp_adc_cal.h"

//#include "driver/i2s.h"
//#include "freertos/queue.h"

class AudioI2S
{
public:
    AudioI2S() = default;
    ~AudioI2S() = default;

    void begin();
    void update();
};

