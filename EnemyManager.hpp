#pragma once
#include <vector>
#include <random>
#include <raylib.h>
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Animation.hpp"
class EnemyManager
{
public:
	EnemyManager();

	bool reached_player(unsigned short i_player_y) const;

	void draw(raylib::DrawSession& ds);
	void reset(unsigned short i_level);
	void update(std::mt19937_64& i_random_engine);

	std::vector<Bullet>& get_enemy_bullets();
	std::vector<Enemy>& get_enemies();
private:
	//I didn't wanna use floats, so I did this. The enemies will pause for a few frames and then move. To make them faster, we'll just reduce the duration of the pause.
	unsigned short _move_pause;
	//Pause timer
	unsigned short _move_timer;

	//To use the randomness from the <random> library, we need to define the distribution.
	std::uniform_int_distribution<unsigned short> _shoot_distribution;
	std::vector<Animation> _enemy_animations;
	std::vector<Bullet> _enemy_bullets;
	std::vector<Enemy> _enemies;

	Texture2D _enemy_bullet_sprite;

	Sound _enemymove;
	Sound _enemydestroy;
};