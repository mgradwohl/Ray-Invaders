// Corresponding header
#include "Ufo.hpp"

// Standard library headers
#include <algorithm>
#include <random>
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Animation.hpp"
#include "Collision.hpp"
#include "Global.hpp"
#include "RLWaveSound.hpp"

constexpr float UFO_EXPLOSION_Y_OFFSET = 0.5F;
constexpr float UFO_EXPLOSION_MAGIC_OFFSET = 0.5F;

Ufo::Ufo(std::mt19937_64 &i_random_engine)
    : _y(GlobalConstant::BASE_SIZE), _powerup_distribution(0, GlobalConstant::Int::POWERUP_TYPES - 1),
      _timer_distribution(GlobalConstant::Int::UFO_TIMER_MIN, GlobalConstant::Int::UFO_TIMER_MAX),
      _animation(GlobalConstant::Int::UFO_ANIMATION_SPEED, 2 * GlobalConstant::BASE_SIZE, "Resources/Images/Ufo.png"),
      _explosion(GlobalConstant::Int::EXPLOSION_ANIMATION_SPEED, 2 * GlobalConstant::BASE_SIZE, "Resources/Images/ExplosionBig.png"),
      _ufoappearsound("Resources/Sounds/UFO Enter.wav"), _ufodestroysound("Resources/Sounds/UFO Destroy.wav")
{
    // Avoid vector growth during powerup creation and updates
    _powerup_animations.reserve(GlobalConstant::Int::POWERUP_TYPES);
    _powerups.reserve(8);
    reset(true, i_random_engine);
    for (GameTypes::Count puType = 0; puType < GlobalConstant::Int::POWERUP_TYPES; puType++)
    {
        // Use implicit conversion to create the string
        std::string powerupIndex = std::to_string(puType);
        _powerup_animations.emplace_back(GlobalConstant::Int::POWERUP_ANIMATION_SPEED, GlobalConstant::BASE_SIZE,
                                         "Resources/Images/Powerup" + powerupIndex + ".png");
    }
}

auto Ufo::check_bullet_collision(std::mt19937_64 &i_random_engine, const Rectangle &i_bullet_hitbox) -> bool
{
    if (!_dead)
    {
        if (AabbIntersect(get_hitbox(), i_bullet_hitbox))
        {
            _dead = true;
            _ufoappearsound.Stop();
            _explosion_x = _x;
            [[maybe_unused]] bool played = _ufodestroysound.Play();

            // Get the powerup type from the distribution and cast to enum
            GameTypes::Count powerupIndex = _powerup_distribution(i_random_engine) % GlobalConstant::Int::POWERUP_TYPES;
            auto powerupType = static_cast<PowerUpItem::Type>(powerupIndex);
            _powerups.emplace_back(_x + (0.5F * GlobalConstant::BASE_SIZE), _y, powerupType);

            return true;
        }
    }

    return false;
}

auto Ufo::check_powerup_collision(const Rectangle &i_player_hitbox) noexcept -> GameTypes::Count
{
    for (PowerUpItem &powerup : _powerups)
    {
        if (!powerup.isdead() && AabbIntersect(powerup.get_hitbox(), i_player_hitbox))
        {
            powerup.isdead(true);

            // Plus 1, because 0 means we didn't pick up any powerups.
            return 1 + static_cast<GameTypes::Count>(powerup.get_type());
        }
    }

    return 0;
}

void Ufo::draw(raylib::DrawSession &ds) const
{
    if (!_dead)
    {
        _animation.draw(ds, _x, _y, GlobalColors::COL_WHITE);
    }
    else if (!_dead_animation_over)
    {
        // Only draw the explosion animation until it's finished
        _explosion.draw(ds, _explosion_x, _y - (UFO_EXPLOSION_Y_OFFSET * GlobalConstant::BASE_SIZE), GlobalColors::EXPLOSION_ORANGE_RED);
        _explosion.draw(ds, _explosion_x, (_y - (UFO_EXPLOSION_MAGIC_OFFSET * GlobalConstant::BASE_SIZE)),
                        GlobalColors::EXPLOSION_ORANGE_RED);
        _explosion.draw(ds, _explosion_x, _y - (0.5F * GlobalConstant::BASE_SIZE), GlobalColors::EXPLOSION_ORANGE_RED);
    }

    for (const PowerUpItem &powerup : _powerups)
    {
        _powerup_animations[static_cast<int>(powerup.get_type())].draw(ds, powerup.get_x(), powerup.get_y(), GlobalColors::COL_WHITE);
    }
}

void Ufo::reset(bool i_dead, std::mt19937_64 &i_random_engine)
{
    _dead = i_dead;
    _dead_animation_over = false;

    _explosion_x = GlobalConstant::SCREEN_WIDTH;
    _x = GlobalConstant::SCREEN_WIDTH;

    _timer = _timer_distribution(i_random_engine);

    _powerups.clear();
    _ufoappearsound.Stop();
    _ufodestroysound.Stop();
    _animation.reset();
    _explosion.reset();
}

void Ufo::update(std::mt19937_64 &i_random_engine)
{
    if (!_dead)
    {
        if (!_ufoappearsound.IsPlaying())
        {
            [[maybe_unused]] bool played = _ufoappearsound.Play();
        }

        _x -= GlobalConstant::UFO_MOVE_SPEED;

        // As soon as the UFO leaves the screen, it gets destroyed. But no powerups will appear.
        if (_x <= -2 * GlobalConstant::BASE_SIZE)
        {
            _dead = true;
            _ufoappearsound.Stop();
        }

        _animation.update();
    }
    else
    {
        if (!_dead_animation_over)
        {
            _dead_animation_over = _explosion.update();
        }

        if (_timer == 0)
        {
            reset(false, i_random_engine);
        }
        else
        {
            _timer--;
        }
    }
    for (PowerUpItem &powerup : _powerups)
    {
        powerup.bump_y(GlobalConstant::POWERUP_SPEED);
        if (GlobalConstant::SCREEN_HEIGHT <= powerup.get_y())
        {
            powerup.isdead(true);
        }
    }

    for (Animation &powerup_animation : _powerup_animations)
    {
        powerup_animation.update();
    }

    // Erase-remove idiom: remove dead powerups from the vector
    _powerups.erase(std::remove_if(_powerups.begin(), _powerups.end(),
                                   [](const PowerUpItem &i_powerup) noexcept {
                                       return i_powerup.isdead();
                                   }),
                     _powerups.end());
}

auto Ufo::get_hitbox() const noexcept -> Rectangle
{
    return Rectangle{_x, _y, 2 * GlobalConstant::BASE_SIZE, GlobalConstant::BASE_SIZE};
}
