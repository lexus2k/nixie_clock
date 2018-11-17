#pragma once

#include "nixie_tube.h"
#include "fake_group_controller.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <string.h>
#include <functional>

class NixieDisplay
{
public:
    NixieDisplay() = default;
    ~NixieDisplay() = default;

    NixieTubeAnimated& operator [](int index);

    void set(const char *p);
    void set_effect( Effect effect );
#if 0
    // TODO
    void swipe_left(const char *p);
    void swipe_right(const char *p);
#endif

    void on();
    void off(); // TODO: implement smooth off
    void set_brightness(uint8_t brightness);

    virtual void begin();
    virtual void end();
    virtual void update();

protected:
    virtual NixieTubeAnimated* get_by_index(int index) = 0;
    void do_for_each(const std::function<void(NixieTubeAnimated &tube)> &func);

private:
    NixieTube  m_fake_tube;
};

