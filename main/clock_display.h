#include "nixie_display.h"
#include "spibus.h"
#include "hv5812_group_controller.h"
#include "pwm_group_controller.h"
#include <vector>

#pragma once

class CustomNixieDisplay: public NixieDisplay
{
public:
    CustomNixieDisplay();
    ~CustomNixieDisplay();

    void setup_in14();
    void setup_in12a();
    void begin() override;
    void update() override;
    void end() override;

protected:
    NixieTubeAnimated* get_by_index(int index) override;

private:
	std::vector<NixieTubeAnimated *> m_tubes;
    PinGroupControllerHv5812 m_cathodes;
    PinGroupControllerPwm m_anods;
};

