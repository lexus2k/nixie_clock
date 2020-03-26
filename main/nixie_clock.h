#pragma once

#include "sm_engine2.h"
#include "controllers/mqtt_controller.h"

class NixieClock: public SmEngine2
{
public:
    NixieClock();

    EEventResult on_event(SEventData event) override;

    void on_update() override;

    bool on_begin() override;

    void on_end() override;

    uint64_t get_micros() override;

private:
     MqttSensorController m_mqtt;

     void on_wifi_connected( bool staMode );

     void on_wifi_disconnected( bool staMode );

     void on_upgrade_status( uintptr_t status );

     void on_check_new_fw();
};

