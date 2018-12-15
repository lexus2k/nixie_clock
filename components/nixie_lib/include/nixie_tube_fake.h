#pragma once
#include "nixie_tube_animated.h"

class NixieTubeFake: public NixieTubeAnimated
{
public:
    const char * set(const char *p) override { return p; }
};