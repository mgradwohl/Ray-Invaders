#pragma once
#include <random>

#include <raylib.h>

#include "Animation.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "RLDrawSession.hpp"
#include "RLTexture2D.hpp"
#include "RLWaveSound.hpp"
#include "Ufo.hpp"

class HitManager; // forward declaration
class Player
{
  public:
    Player();

    [[nodiscard]] auto get_dead() const noexcept -> bool;
    [[nodiscard]] auto get_dead_animation_over() const noexcept -> bool;

    [[nodiscard]] auto get_current_power() const noexcept -> GameTypes::Count;

    [[nodiscard]] auto get_power_timer() const noexcept -> GameTypes::Duration;
    [[nodiscard]] auto get_y() const noexcept -> float;

    void die() noexcept;
    void draw(raylib::DrawSession &ds) const;
    void reset();
    void update(std::mt19937_64 &i_random_engine, std::vector<Bullet> &i_enemy_bullets, std::vector<Enemy> &i_enemies, Ufo &i_ufo,
                HitManager &i_hits);

    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    auto get_player_bullets() noexcept -> std::vector<Bullet> &
    {
        return _bullets;
    }

  private: // Is it okay to call this variable "dead"?
    // I mean, it's a spaceship.
    // And spaceships don't die, they get destroyed.
      // _destroyed indicates the player is currently destroyed and the death
    // explosion animation should play. _dead (final out-of-lives) is derived
    // from the lives counter via get_dead().
    bool _destroyed{false};
      bool _dead_animation_over{false};
  bool _shield_animation_over{false};

    GameTypes::Count _current_power{0};
    GameTypes::Timer _reload_timer{0};

    GameTypes::Duration _power_timer{0};
    float _x{0.0F};
    float _y{0.0F};

  // Number of remaining lives for this player (initialized to default via
  // GlobalConstant::Int::INITIAL_LIVES)
  int _lives{GlobalConstant::Int::INITIAL_LIVES};

    std::vector<Bullet> _bullets;

    raylib::Texture2DFile _bullet_sprite;
    raylib::Texture2DFile _player_sprite;
    raylib::WaveSound _playerlasersound;
    raylib::WaveSound _powerupsound;
    raylib::WaveSound _playerdestroysound;
    raylib::WaveSound _playershieldsound;

    Animation _explosion;

  public:
    // Lives management API
    [[nodiscard]] auto get_lives() const noexcept -> int;
    void set_lives(int lives) noexcept;
    void add_lives(int n) noexcept;
    // Decrement lives by one and return remaining lives
    auto lose_life() noexcept -> int;
};