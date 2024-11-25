//SO MANY CONSTANTS! AAAAAAAH!
#pragma once

#include <cmath>
#include <array>
#include <chrono>

//I didn't wanna make PLAYER_SIZE, ENEMY_SIZE, BULLET_SIZE, so I just defined the base size.
constexpr unsigned char BASE_SIZE = 16;
constexpr unsigned char ENEMY_BULLET_SPEED = 2;
//I explained this in "Enemy.hpp". AND I DON'T LIKE TO REPEAT MYSELF!
constexpr unsigned char ENEMY_HIT_TIMER_DURATION = 2;
//The more enemies we kill, the faster they become.
constexpr unsigned char ENEMY_MOVE_PAUSE_DECREASE = 1;
//Setting this to 1 or even 0 results in terrible enemy movement. So don't do it.
//...
//...
//You're gonna do it anyway, aren't you?
constexpr unsigned char ENEMY_MOVE_PAUSE_MIN = 3;
//"Why not 64?" - you may ask. Well, it's because if we set it to 0, they will still have 1 frame pause. So 63 + 1 = 64.
constexpr unsigned char ENEMY_MOVE_PAUSE_START = 63;
constexpr unsigned char ENEMY_MOVE_PAUSE_START_MIN = 47;
constexpr unsigned char ENEMY_MOVE_SPEED = 2;
constexpr unsigned char ENEMY_TYPES = 3;
constexpr unsigned char EXPLOSION_ANIMATION_SPEED = 2;
//Same logic with ENEMY_MOVE_PAUSE_START.
constexpr unsigned char FAST_RELOAD_DURATION = 7;
//The transition to the next level will last 64 frames or about 1 second.
constexpr unsigned char NEXT_LEVEL_TRANSITION = 64;
constexpr unsigned char PLAYER_BULLET_SPEED = 4;
constexpr unsigned char PLAYER_MOVE_SPEED = 2;
constexpr unsigned char POWERUP_ANIMATION_SPEED = 16;
constexpr unsigned char POWERUP_SPEED = 2;
constexpr unsigned char POWERUP_TYPES = 4;
constexpr unsigned char RELOAD_DURATION = 31;
constexpr unsigned char SCREEN_RESIZE = 5;
constexpr unsigned char TOTAL_LEVELS = 8;
constexpr unsigned char UFO_ANIMATION_SPEED = 8;
constexpr unsigned char UFO_MOVE_SPEED = 1;
constexpr unsigned char BASE_COUNT = 4;
constexpr unsigned char BASE_WIDTH = 30;

//There are 64 enemies. So the probability that at least one of them shoots is 1 - (1 - 1 / 4096)^64 = 1.55%
//Keep in mind that we do this every frame.
constexpr unsigned short ENEMY_SHOOT_CHANCE = 4096;
constexpr unsigned short ENEMY_SHOOT_CHANCE_INCREASE = 64;
constexpr unsigned short ENEMY_SHOOT_CHANCE_MIN = 1024;
constexpr unsigned short POWERUP_DURATION = 512;
constexpr unsigned short SCREEN_HEIGHT = 180;
constexpr unsigned short SCREEN_WIDTH = 320;
//The UFO will appear after this number of frames.
constexpr unsigned short UFO_TIMER_MAX = 1024;
constexpr unsigned short UFO_TIMER_MIN = 768;

constexpr std::chrono::microseconds FRAME_DURATION(16667);

struct Powerup
{
public:
	Powerup(float i_x, float i_y, unsigned char i_type) :
		_dead(0),
		_x(i_x),
		_y(i_y),
		_type(i_type)
	{
		
	}

	const bool isdead() const
	{
		return _dead;
	}

	void isdead(bool dead)
	{
		_dead = dead;
	}

	float getx()
	{
		return _x;
	}

	float gety()
	{
		return _y;
	}

	void bump_y(unsigned short ybump)
	{
		_y += ybump;
	}

	Rectangle get_hitbox() const
	{
		return Rectangle(_x + 0.25f * BASE_SIZE, _y + 0.25f * BASE_SIZE, 0.5f * BASE_SIZE, 0.5f * BASE_SIZE);
	}

	unsigned char get_type()
	{
		return _type;
	}
private:
	//0 - Shield
	//1 - Fast reload
	//2 - 3 bullets
	//3 - Mirrored controls (power-DOWN! I'm so proud of this joke)
	//Damn, I should've used enums. That would've made the code more readable.
	unsigned char _type;
	bool _dead;

	float _x;
	float _y;
};