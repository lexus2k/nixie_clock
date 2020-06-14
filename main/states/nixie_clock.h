#pragma once

#include "sme/engine.h"
#include "controllers/mqtt_controller.h"

class NixieClock: public SmEngine
{
public:
    NixieClock();

    STransitionData onEvent(SEventData event) override;

    void onUpdate() override;

    bool onBegin() override;

    void onEnd() override;

    uint64_t getMicros() override;

private:
     MqttSensorController m_mqtt;

     void on_wifi_connected( bool staMode );

     void on_wifi_disconnected( bool staMode );

     void on_upgrade_status( uintptr_t status );

     void on_check_new_fw();
};

