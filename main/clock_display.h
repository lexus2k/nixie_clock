#include "nixie_display.h"
#include "spibus.h"
#include "hv5812_group_controller.h"

#pragma once

class CustomNixieDisplay: public NixieDisplay
{
public:
    CustomNixieDisplay();

    void begin() override;
    void update() override;
    void end() override;

protected:
    NixieTubeAnimated* get_by_index(int index) override;

private:
    NixieTube m_tubes[6];
    PinGroupControllerHv5812 m_cathodes;
};

