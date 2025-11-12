#pragma once

#include <raylib.h>

#include "Animation.hpp"
#include "Global.hpp"
#include "PowerUpItem.hpp"
#include "RLDrawSession.hpp"
#include "RLWaveSound.hpp"

class Ufo
{
  public:
    Ufo();

    auto check_bullet_collision(const Rectangle &i_bullet_hitbox) -> bool;

    auto check_powerup_collision(const Rectangle &i_player_hitbox) noexcept -> GameTypes::Count;
    void draw(raylib::DrawSession &ds) const;
    void reset(bool i_dead);
    void update();

    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;

  private:
    bool _dead{false};
    bool _dead_animation_over{false};
    GameTypes::Duration _timer{0};

    float _explosion_x{0.0F};
    float _x{0.0F};
    float _y{0.0F};

    std::uniform_int_distribution<int> _powerup_distribution; // Changed from GameTypes::Count to int for Clang compatibility
    // The UFO can appear after 12 seconds. Or 16 seconds. Or 14.652 seconds. We don't know for
    // sure.
    std::uniform_int_distribution<int> _timer_distribution; // Changed from GameTypes::Duration to int for Clang compatibility
    std::vector<Animation> _powerup_animations;
    // Yes, the UFO is responsible for the powerups too.
    std::vector<PowerUpItem> _powerups;

    Animation _animation;
    Animation _explosion;
    raylib::WaveSound _ufoappearsound;
    raylib::WaveSound _ufodestroysound;
};