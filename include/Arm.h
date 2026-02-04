#pragma once

#include <cstddef>
#include <iostream>
#include "PointEngine.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Weapon.h"
#include "globals.hpp"

class Arm
{
    public:
        Arm()
        {

        }
        Arm(PointEngine& pe, sf::Vector2f pos, sf::Vector2f offset, size_t count, int handIndex)
        :pe(&pe)
        {
            std::cout << "reached the Arm constructor\n";
            for(size_t i = 0; i < count; i++)
            {
                pe.addPoint(pos + offset * (float)i, false, false, 5.f , PLAYER_FRICTION, 1000 - 100 * i);
                pe.getPoint(pe.getPointCount()-1).setGravityScale(0.1);
                ropeIndexes.emplace_back(pe.getPointCount()-1);
                unsigned int index = ropeIndexes[i];
                if(i > 0)
                {
                    pe.addConstraint(ropeIndexes[i], ropeIndexes[i-1], PointEngine::DISTANCE_CONSTRAINT_MINMAX, 12.f);
                    std::cout << ropeIndexes[i] << ";" << ropeIndexes[i-1] << std::endl;
                }
                else
                    pe.addConstraint(index, handIndex, PointEngine::DISTANCE_CONSTRAINT_MINMAX, 5.f);
            }
        }
        void shoot()
        {

        }

        void hardMoveGroup(sf::Vector2f offset)
        {
            for(int i = 0; i < ropeIndexes.size(); i++)
            {
                pe->getPoint(ropeIndexes[i]).move(offset, true);
                pe->getPoint(ropeIndexes[i]).setOldPos(pe->getPoint(ropeIndexes[i]).getPos());
            }
        }

    private:
        bool retracted = false;
        sf::Texture spriteTex;
        sf::RectangleShape sprite;
        std::vector<int> ropeIndexes;
        PointEngine* pe;
        //std::unique_ptr<Weapon> weapon;

};