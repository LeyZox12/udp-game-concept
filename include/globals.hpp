#pragma once

inline const float PLAYER_SPEED = 5.4f;
inline const float PLAYER_FRICTION = 60.f;
inline const float PLAYER_RADIUS = 15.f;
inline const int BLOCK_SIZE = 30;

enum PACKET_TYPE
{
	CLIENT_IN,
	CLIENT_OUT,
	MESSAGE,
	UDP_PORT,
	CLIENT_MOVE,
	CLIENT_POSITIONS,
	MAP_TRANSFER_INFO
};

enum BLOCK_TYPE
{
	VOID,
	HARD,
	PLATFORM,
	DEATH,
	SPAWN,
	ITEM,
	SPEED
};
