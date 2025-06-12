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
	EnemyManager() noexcept;

	[[nodiscard]] bool reached_player(float i_player_y) const;

	void draw(raylib::DrawSession& ds);
	void reset(unsigned short i_level);
	void update(std::mt19937_64& i_random_engine);

	std::vector<Bullet>& get_enemy_bullets() noexcept;
	std::vector<Enemy>& get_enemies() noexcept;
private:
	// Change to int to avoid casting between unsigned short and int in calculations
	int _move_pause{0};
	//Pause timer
	int _move_timer{0};

	//To use the randomness from the <random> library, we need to define the distribution.
	std::uniform_int_distribution<unsigned short> _shoot_distribution;
	std::vector<Animation> _enemy_animations{};
	std::vector<Bullet> _enemy_bullets{};
	std::vector<Enemy> _enemies{};

	Texture2D _enemy_bullet_sprite{};

	Sound _enemymove{};
	Sound _enemydestroy{};
};