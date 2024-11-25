#pragma once
#include <random>
#include <raylib.h>
#include "RLDrawSession.h"
#include "Animation.hpp"
#include "Ufo.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"
class Player
{
public:
	Player();

	bool get_dead() const;
	bool get_dead_animation_over() const;

	unsigned char get_current_power() const;

	unsigned short get_power_timer() const;
	float get_y() const;

	void die();
	void draw(raylib::DrawSession& ds);
	void reset();
	void update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo);

	Rectangle get_hitbox() const;
	std::vector<Bullet>& get_player_bullets()
	{
		return _bullets;
	}

private:
	//Is it okay to call this variable "dead"?
	//I mean, it's a spaceship.
	//And spaceships don't die, they get destroyed.
	bool _dead;
	bool _dead_animation_over;
	bool _shield_animation_over;

	unsigned char _current_power;
	unsigned char _reload_timer;

	unsigned short _power_timer;
	float _x;
	float _y;

	std::vector<Bullet> _bullets;

	Texture2D _bullet_sprite;
	Texture2D _player_sprite;
	Sound _playerlasersound;
	Sound _powerupsound;
	Sound _playerdestroysound;

	Animation _explosion;
};