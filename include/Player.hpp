#pragma once

#include <SFML/Graphics.hpp>
#include <PointEngine.h>
#include "globals.hpp"

using namespace sf;

struct Player
{

	Player(PointEngine& pe)
	{
		point = pe.addPoint(vec2(100, 0), false, true, PLAYER_RADIUS, PLAYER_FRICTION);
		point->setGravityScale(0.3f);
	}
	float rot = 0.f;
	float hp = 100.f;
	
	Point* point;
	View camera = View(Vector2f(512, 512), Vector2f(512, 512));
};