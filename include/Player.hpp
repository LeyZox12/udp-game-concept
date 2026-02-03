#pragma once

#include <SFML/Graphics.hpp>
#include <PointEngine.h>
#include "SFML/System/Vector2.hpp"
#include "globals.hpp"

using namespace sf;

struct Player
{

	Player(PointEngine& pe)
	{
		point = pe.addPoint(vec2(100, 0), false, true, PLAYER_RADIUS, PLAYER_FRICTION);
		point->setGravityScale(0.3f);
	}

	//WARNING, POINT IS NULL POINTER
	Player()
	{
	}

	Vector2f leftHand()
	{
		return point->getPos() - Vector2f(point->getRadius(), 0.f);
	}	

	Vector2f rightHand()
	{
		return point->getPos() - Vector2f(point->getRadius(), 0.f);
	}	

	float rot = 0.f;
	float hp = 100.f;
	int id = 0;
	
	Point* point;
	View camera = View(Vector2f(512, 512), Vector2f(512, 512));
};