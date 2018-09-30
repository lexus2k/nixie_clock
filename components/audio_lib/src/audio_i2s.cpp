#include "audio_i2s.h"
#include "audio_notes_decoder.h"
#include "nixie_melodies.h"
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

void AudioI2S::begin()
{
    i2s_config_t i2s_config{};
    i2s_config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);
    i2s_config.sample_rate = 16000;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_ALL_RIGHT;
    i2s_config.communication_format = static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S_MSB);
    i2s_config.intr_alloc_flags = 0; //ESP_INTR_FLAG_LEVEL1;
    i2s_config.dma_buf_count = 8;
    i2s_config.dma_buf_len = 64;
    i2s_config.use_apll = false;
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK)
    {
        printf("error: %i\n", err);
    }
//    i2s_set_pin(I2S_NUM_0, NULL);        
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
    i2s_set_sample_rates(I2S_NUM_0, 16000);
    i2s_zero_dma_buffer( I2S_NUM_0 );
}

void AudioI2S::update()
{
    AudioNotesDecoder decoder;
    decoder.set_melody( &melodyMonkeyIslandP );
    decoder.set_format(16000, 16);
    while (1)
    {
        size_t written = 0;
        int size = decoder.decode();
        uint8_t* buffer = decoder.get_buffer();
        esp_err_t err = i2s_write(I2S_NUM_0, buffer, size, &written, 100);
        if (size != 0) printf("Audio decoded size %i, written %i\n", size, written);
        if (err != ESP_OK)
        {
            printf("Audio error\n");
        }
        if (size == 0)
        {
            i2s_zero_dma_buffer( I2S_NUM_0 );
        }
//        vTaskDelay(100/portTICK_RATE_MS);
    }
}

void AudioI2S::end()
{
    i2s_driver_uninstall(I2S_NUM_0);
}


/*
...

    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

    i2s_set_pin(i2s_num, NULL); //for internal DAC, this will enable both of the internal channels

    //You can call i2s_set_dac_mode to set built-in DAC output mode.
    //i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

    i2s_set_sample_rates(i2s_num, 22050); //set sample rates

    i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

*/
