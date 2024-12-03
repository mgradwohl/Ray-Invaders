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

	[[nodiscard]] bool get_dead() const noexcept;
	[[nodiscard]] bool get_dead_animation_over() const noexcept;

	[[nodiscard]] unsigned char get_current_power() const noexcept;

	[[nodiscard]] unsigned short get_power_timer() const noexcept;
	[[nodiscard]] float get_y() const noexcept;

	void die() noexcept;
	void draw(raylib::DrawSession& ds);
	void reset();
	void update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo);

	[[nodiscard]] Rectangle get_hitbox() const noexcept;
	std::vector<Bullet>& get_player_bullets() noexcept
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