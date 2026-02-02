#pragma once

#include <iostream>
#include "PointEngine.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Arm
{
    public:
        virtual void onUse() = 0;
    private:
        bool retracted = false;
        sf::Texture spriteTex;
        sf::RectangleShape sprite;
        std::vector<Point*> rope;
};