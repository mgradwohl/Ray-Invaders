#pragma once
#include <string>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"

class Ufo
{
public:
	Ufo(std::mt19937_64& i_random_engine);

	bool check_bullet_collision(std::mt19937_64& i_random_engine, const Rectangle& i_bullet_hitbox);

	unsigned char check_powerup_collision(const Rectangle& i_player_hitbox);

	void draw(raylib::DrawSession& ds);
	void reset(bool i_dead, std::mt19937_64& i_random_engine);
	void update(std::mt19937_64& i_random_engine);

	Rectangle get_hitbox() const;
private:
	bool dead;
	bool dead_animation_over;

	short explosion_x;
	short x;

	unsigned short timer;
	unsigned short y;

	std::uniform_int_distribution<unsigned short> powerup_distribution;
	//The UFO can appear after 12 seconds. Or 16 seconds. Or 14.652 seconds. We don't know for sure.
	std::uniform_int_distribution<unsigned short> timer_distribution;

	std::vector<Animation> powerup_animations;

	//Yes, the UFO is responsible for the powerups too.
	std::vector<Powerup> powerups;

	Animation animation;
	Animation explosion;
	raylib::WaveSound ufoappearsound;
};