#pragma once
#include <vector>

// Project headers
#include "Animation.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "RLTexture2D.hpp"
#include "RLWaveSound.hpp"

class EnemyManager
{
  public:
    EnemyManager() noexcept;

    [[nodiscard]] auto reached_player(float i_player_y) const -> bool;

    void draw(raylib::DrawSession &ds) const;
    void reset(GameTypes::Level i_level);
    void update();

    auto get_enemy_bullets() noexcept -> std::vector<Bullet> &;
    auto get_enemies() noexcept -> std::vector<Enemy> &;

  private:
    // Change to int to avoid casting between unsigned short and int in calculations
    int _move_pause{0};
    // Pause timer
    int _move_timer{0};

    // Uses XOSHIRO random number generator via Random utility.
    std::uniform_int_distribution<GameTypes::Probability> _shoot_distribution;
    std::vector<Animation> _enemy_animations;
    std::vector<Bullet> _enemy_bullets;
    std::vector<Enemy> _enemies;

    raylib::Texture2DFile _enemy_bullet_sprite;

    raylib::WaveSound _enemymove;
    raylib::WaveSound _enemydestroy;
};