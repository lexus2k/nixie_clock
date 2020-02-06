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
};

