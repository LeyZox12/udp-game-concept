#pragma once

#include "PointEngine.h"
#include <SFML/Graphics.hpp>
#include "Arm.h"

class Hook : public Arm
{
    public:
        Hook();
        void onUse();
};