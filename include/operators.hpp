#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <PointEngine.h>
#include <cstddef>
#include <memory>
#include "PhysicConstraint.h"
#include "Player.hpp"
#include "SFML/System/Vector2.hpp"

using namespace sf;


inline Packet& operator<<(Packet& p, Point& point)
{
	return p 
	<< point.getAcc().x 
	<< point.getAcc().y 
	<< point.getColor().r 
	<< point.getColor().g
	<< point.getColor().a
	<< point.getGravityScale()
	<< point.getIsStatic()
	<< point.getOldPos().x
	<< point.getOldPos().y
	<< point.getPos().x
	<< point.getPos().y
	<< point.getRadius()
	<< point.getShouldCollide();
}

inline Packet& operator>>(Packet& p, Point& point)
{
    Vector2f acc;
    Vector2f pos;
    Vector2f oldPos;
    Color col;

    float friction;
    float gravityScale;
    float radius;
    bool shouldCollide;
    bool isStatic;

    p 
    >> acc.x
    >> acc.y
    >> col.r 
    >> col.g
    >> col.b 
    >> col.a 
    >> friction
    >> gravityScale
    >> isStatic
    >> oldPos.x
    >> oldPos.y
    >> pos.x
    >> pos.y
	>> radius
    >> shouldCollide;
	point.setPos(pos, true);
	point.setAcc(acc);
	point.setColor(Color(col.r, col.g, col.b, col.a));
	point.setOldPos(oldPos);
	point.setGravityScale(gravityScale);
	point.setRadius(radius);
	point.setIsStatic(isStatic);
	return p;
}

inline Packet& operator<<(Packet& p, PhysicConstraint &pc)
{
	return p 
	<< pc.getDist()
	<< pc.getIndexes().first
	<< pc.getIndexes().second
	<< pc.getType()
	<< pc.getVisible();
}

inline Packet& operator>>(Packet& p, PhysicConstraint &pc)
{
	float dist;
	int i1, i2, type;
	bool visible;
	p 
	>> dist
	>> i1
	>> i2
	>> type
	>> visible;
	pc.setType(type);
	pc.setVisible(visible);
	pc.setDistance(dist);
	pc.setIndex1(i1);
	pc.setIndex2(i2);
	return p;
}

//not sending rects, 
// they need to be built by client and server when map is loaded
inline Packet& operator<<(Packet& p, PointEngine &pe)
{
	p << pe.getPointCount();
	for(int i = 0; i < pe.getPointCount(); i++)
		p << pe.getPoint(i);
	p << pe.getConstraintCount();
	for(int i = 0; i < pe.getConstraintCount(); i++)
		p << pe.getConstraint(i);
	return p;
}

inline Packet& operator>>(Packet& p, PointEngine &pe)
{
	size_t pointCount, constraintCount;
	p >> pointCount;
	for(int i = 0; i < pointCount; i++)
	{
		Point point;
		p >> point;
		pe.addPoint(
			point.getPos(), 
			point.getIsStatic(), 
			point.getShouldCollide(), 
			point.getRadius(), 
			point.getMass()//not sending this
		);

	}
	p >> constraintCount;
	for(int i = 0; i < constraintCount; i++)
	{
		PhysicConstraint constraint;
		p >> constraint;
		pe.addConstraint(
			constraint.getIndexes().first, 
			constraint.getIndexes().second, 
			constraint.getType(), 
			constraint.getDist());
	}
	return p;
}

inline Packet& operator<<(Packet& p, Player& player)
{
	return p 
	<< player.getPos().x
	<< player.getPos().y
	<< player.rot 
	<< player.hp;
}

inline Packet& operator>>(Packet& p, Player& player)
{
	Vector2f pos;
	p 
	>> pos.x
	>> pos.y
	>> player.rot
	>> player.hp;
	player.setPos(pos);
	return p;
}