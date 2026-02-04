#pragma once

#include <SFML/Graphics.hpp>
#include <PointEngine.h>
#include <memory>
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"
#include "globals.hpp"
#include "Arm.h"

using namespace sf;

struct Player
{
	Player(PointEngine& pe, Vector2f pos)
	:pe(pe)
	{
		pe.addPoint(pos, false, false, PLAYER_RADIUS, PLAYER_FRICTION, 1000.f);
		coreIndex = pe.getPointCount()-1;
		pe.getPoint(coreIndex).setGravityScale(0.3);

		std::cout<<"core index" << coreIndex << std::endl;
		leftArm = (Arm(pe, leftHand(), Vector2f(-10, 0), 10, coreIndex)); 
		rightArm = (Arm(pe, rightHand(), Vector2f(10, 0), 10, coreIndex));
	}
	
	Vector2f leftHand()
	{
		return getPos() - Vector2f(PLAYER_RADIUS, 0.f);
	}	

	Vector2f rightHand()
	{
		return getPos() - Vector2f(PLAYER_RADIUS, 0.f);
	}	

	Vector2f getPos()
	{
		return pe.getPoint(coreIndex).getPos();
	}

	void setPos(Vector2f pos)
	{
		pe.getPoint(coreIndex).setPos(pos, false);
	}

	void hardSetPos(Vector2f pos)
	{
		Vector2f diff = pos - pe.getPoint(coreIndex).getPos();
		pe.getPoint(coreIndex).setPos(pos, true);
		pe.getPoint(coreIndex).setOldPos(pos);
		leftArm.hardMoveGroup(diff);
		rightArm.hardMoveGroup(diff);
		
	}

	void setOldPos(Vector2f pos)
	{
		pe.getPoint(coreIndex).setOldPos(pos);
	}

	Point& getPoint()
	{
		return pe.getPoint(coreIndex);
	}	

	float rot = 0.f;
	float hp = 100.f;
	int id = 0;
	
	int coreIndex;

	PointEngine& pe;

	Arm leftArm; 
	Arm rightArm; 

	View camera = View(Vector2f(512, 512), Vector2f(512, 512));
};