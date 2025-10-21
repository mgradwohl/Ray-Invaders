#pragma once
#include <vector>
#include <random>
#include <raylib.h>
#include "RLTexture2D.hpp"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Animation.hpp"
class EnemyManager
{
public:
	EnemyManager() noexcept;

    [[nodiscard]] auto reached_player(float i_player_y) const -> bool;

    void draw(raylib::DrawSession& ds);
	void reset(GameTypes::Level i_level);
	void update(std::mt19937_64& i_random_engine);

    auto get_enemy_bullets() noexcept -> std::vector<Bullet> &;
    auto get_enemies() noexcept -> std::vector<Enemy> &;

  private:
	// Change to int to avoid casting between unsigned short and int in calculations
	int _move_pause{0};
	//Pause timer
	int _move_timer{0};

	//To use the randomness from the <random> library, we need to define the distribution.
	std::uniform_int_distribution<GameTypes::Probability> _shoot_distribution;
    std::vector<Animation> _enemy_animations;
    std::vector<Bullet> _enemy_bullets;
    std::vector<Enemy> _enemies;

    raylib::Texture2DFile _enemy_bullet_sprite;

	raylib::WaveSound _enemymove;
	raylib::WaveSound _enemydestroy;
};