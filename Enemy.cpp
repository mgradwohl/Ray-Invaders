// Corresponding header
#include "Enemy.hpp"

// Standard library headers
#include <algorithm>
#include <cmath>
#include <vector>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Global.hpp"
#include "SoundManager.hpp"

Enemy::Enemy(Type i_type, float i_x, float i_y, GameTypes::Health health) noexcept
    : _health(health), _type(i_type), _x(i_x), // No cast needed with float parameters
      _y(i_y),                                                                             // No cast needed with float parameters
      _enemylaser("Resources/Sounds/Enemy Laser.wav")
{
}

auto Enemy::get_health() const noexcept -> GameTypes::Health
{
    return _health;
}
auto Enemy::get_hit_timer() const noexcept -> GameTypes::Timer
{
    return _hit_timer;
}
[[nodiscard]] auto Enemy::get_type() const noexcept -> Enemy::Type
{
    return _type;
}
auto Enemy::get_x() const noexcept -> float
{
    return _x;
}
auto Enemy::get_y() const noexcept -> float
{
    return _y;
}

void Enemy::hit() noexcept
{
    _hit_timer = GlobalConstant::Int::ENEMY_HIT_TIMER_DURATION;
}

void Enemy::move()
{
    if (_direction != Direction::Down)
    {
        const float rightBoundary = GlobalConstant::SCREEN_WIDTH - (2.0f * GlobalConstant::BASE_SIZE);
        const float leftBoundary = GlobalConstant::BASE_SIZE;

        if ((_direction == Direction::Right && _x >= rightBoundary) || (_direction == Direction::Left && _x <= leftBoundary))
        {
            _direction = Direction::Down;
            _y += GlobalConstant::ENEMY_MOVE_SPEED;
        }
        else
        {
            // Direction enum is defined with underlying type char, which can be converted to float
            // We can avoid the static_cast by using a switch statement or direct mapping
            float moveOffset = 0.0F;

            switch (_direction)
            {
            case Direction::Left:
                moveOffset = -GlobalConstant::ENEMY_MOVE_SPEED;
                break;
            case Direction::Right:
                moveOffset = GlobalConstant::ENEMY_MOVE_SPEED;
                break;
            default: // Direction::Down
                moveOffset = 0.0F;
                break;
            }

            _x = std::clamp<float>(_x + moveOffset, leftBoundary, rightBoundary);
        }
    }
    else
    {
        _y = std::min<float>(_y + GlobalConstant::ENEMY_MOVE_SPEED, GlobalConstant::BASE_SIZE * std::ceil(_y / GlobalConstant::BASE_SIZE));
        if (_y >= (GlobalConstant::BASE_SIZE * std::ceil(_y / GlobalConstant::BASE_SIZE)) - 0.1F)
        {                                                                                  
            const float rowFloat = _y / GlobalConstant::BASE_SIZE;
            // This cast is necessary for the modulo operation which requires an integer
            const int rowInt = static_cast<int>(rowFloat);
            _direction = (rowInt % 2 == 0) ? Direction::Left : Direction::Right;
        }
    }
}

void Enemy::shoot(std::vector<Bullet> &i_enemy_bullets)
{
    // No need for conversions now
    switch (_type)
    {
    case Type::Cyan:
    {
        i_enemy_bullets.emplace_back(0.0F, GlobalConstant::ENEMY_BULLET_SPEED, _x, _y);
        break;
    }
    case Type::Purple:
    {
        i_enemy_bullets.emplace_back(GlobalConstant::EIGHTH * GlobalConstant::ENEMY_BULLET_SPEED, GlobalConstant::ENEMY_BULLET_SPEED, _x, _y);
        i_enemy_bullets.emplace_back(-GlobalConstant::EIGHTH * GlobalConstant::ENEMY_BULLET_SPEED, GlobalConstant::ENEMY_BULLET_SPEED, _x, _y);
        break;
    }
    case Type::Green:
    {
        i_enemy_bullets.emplace_back(0.0F, GlobalConstant::ENEMY_BULLET_SPEED, _x, _y);
        i_enemy_bullets.emplace_back(GlobalConstant::QUARTER * GlobalConstant::ENEMY_BULLET_SPEED, GlobalConstant::ENEMY_BULLET_SPEED, _x, _y);
        i_enemy_bullets.emplace_back(-GlobalConstant::QUARTER * GlobalConstant::ENEMY_BULLET_SPEED, GlobalConstant::ENEMY_BULLET_SPEED, _x, _y);
        break;
    }
    }
    [[maybe_unused]] bool played = _enemylaser.Play();
}

void Enemy::update() noexcept
{
    if (0 < _hit_timer)
    {
        if (1 == _hit_timer)
        {
            // Decrement health directly when it's above 0
            if (_health > 0)
            {
                _health--;
            }
        }

        _hit_timer--;
    }

    // Impact visuals are handled globally by HitManager now.
}

auto Enemy::get_hitbox() const noexcept -> Rectangle
{
    // The sprite width is 16 pixels but the hitbox should be 12 pixels wide, centered
    constexpr float hitbox_width = 12.0F;
    constexpr float x_offset = (GlobalConstant::BASE_SIZE - hitbox_width) * GlobalConstant::HALF; // Center the 12px hitbox within the 16px sprite
    return Rectangle{_x + x_offset, _y + (GlobalConstant::QUARTER * GlobalConstant::BASE_SIZE), hitbox_width, GlobalConstant::HALF * GlobalConstant::BASE_SIZE};
}
