#include "pin_group_controller.h"

void PinGroupController::set(int n, uint8_t pwm)
{
    pwm > 127 ? set( n ) : clear( n );
}
