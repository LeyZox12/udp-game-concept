#pragma once

#include "PointEngine.h"
#include <SFML/Graphics.hpp>

using namespace sf;

class Weapon
{
    Weapon(PointEngine& pe, Vector2f pos)
    {
        //point = pe.addPoint(pos, false, true, 5.f, 100.f);
    }
    virtual void onHit() = 0;
    private:
        Point* point;
};