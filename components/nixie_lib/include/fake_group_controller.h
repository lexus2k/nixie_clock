#pragma once

#include "pin_group_controller.h"

class PinGroupControllerFake: public PinGroupController
{
public:
    using PinGroupController::PinGroupController;

    void begin() override {};

    void end() override {};

    void update() override {};

    void set(int pin) override {};

    void clear(int pin) override {};
};

