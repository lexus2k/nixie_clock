#include "spibus.h"

#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

class Hv5812
{
public:
    Hv5812(WireSPI& spi);
    ~Hv5812();

    bool begin();
    bool write(const uint8_t *data, int len);
    void end();
private:
    WireSPI& m_spi;
    gpio_num_t m_strobe;
};

bool Hv5812::begin()
{
    gpio_set_direction(m_strobe, GPIO_MODE_OUTPUT);
    gpio_set_level(m_strobe, 0);
    return true;
}

void Hv5812::end()
{
}


bool Hv5812::write(const uint8_t *data, int len)
{
    m_spi.beginTransaction( 1000000, -1, 0);
    m_spi.transfer(data, len);
    m_spi.endTransaction();
    ets_delay_us(1);
    gpio_set_level(m_strobe, 1);
    ets_delay_us(1);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(m_strobe, 0);
    return true;
}
