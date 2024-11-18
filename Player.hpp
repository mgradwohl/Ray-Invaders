#pragma once
#include <random>
#include <raylib.h>
#include "RLDrawSession.h"
#include "Animation.hpp"
#include "Ufo.hpp"

class Player
{
	//Is it okay to call this variable "dead"?
	//I mean, it's a spaceship.
	//And spaceships don't die, they get destroyed.
	bool dead;
	bool dead_animation_over;
	bool shield_animation_over;

	unsigned char current_power;
	unsigned char reload_timer;

	unsigned short power_timer;
	unsigned short x;
	unsigned short y;

	std::vector<Bullet> bullets;

	Texture2D bullet_sprite;
	Texture2D player_sprite;
	Sound playerlaser;
	Sound powerup;

	Animation explosion;
public:
	Player();

	bool get_dead() const;
	bool get_dead_animation_over() const;

	unsigned char get_current_power() const;

	unsigned short get_power_timer() const;
	unsigned short get_y() const;

	void die();
	void draw(raylib::DrawSession& ds);
	void reset();
	void update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo);

	Rectangle get_hitbox() const;
};