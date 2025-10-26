// Corresponding header
#include "Bullet.hpp"

// Third-party headers
#include <raylib.h>

// Project headers
#include "Global.hpp"

Bullet::Bullet(float i_step_x, float i_step_y, float i_x, float i_y) noexcept
    : _x(i_x), _y(i_y), _dead(false), _real_x(i_x), _real_y(i_y), _step_x(i_step_x), _step_y(i_step_y)
{
    _previous_x.fill(_x);
    _previous_y.fill(_y);
}

bool Bullet::IsDead() const noexcept
{
    return _dead;
}
void Bullet::IsDead(bool dead) noexcept
{
    _dead = dead;
}

void Bullet::update() noexcept
{
    if (!_dead)
    {
        // I hate using floats, so we'll change real_x and _y and work only with integer values.
        _real_x += _step_x;
        _real_y += _step_y;

        for (GameTypes::Count a = 0; a < _previous_x.size() - 1; a++)
        {
            _previous_x[a] = _previous_x[1 + a];
            _previous_y[a] = _previous_y[1 + a];
        }

        _previous_x[_previous_x.size() - 1] = _x;
        _previous_y[_previous_y.size() - 1] = _y;

        _x = _real_x; // no need for cast, both are float now
        _y = _real_y; // no need for cast, both are float now

        if (_x <= -GlobalConstant::BASE_SIZE || _y <= -GlobalConstant::BASE_SIZE || _y >= GlobalConstant::SCREEN_HEIGHT ||
            _x >= GlobalConstant::SCREEN_WIDTH)
        {
            _dead = true;
        }
    }
}

namespace
{
constexpr float BULLET_HITBOX_WIDTH = 4.0F;
constexpr float BULLET_HITBOX_HEIGHT = GlobalConstant::BASE_SIZE / 2.0F;
constexpr float BULLET_HITBOX_X_OFFSET = (GlobalConstant::BASE_SIZE - BULLET_HITBOX_WIDTH) * GlobalConstant::HALF;
} // namespace

Rectangle Bullet::get_hitbox() const noexcept
{
    // Smaller hitboxes make the game so much better!
    return Rectangle{_x + BULLET_HITBOX_X_OFFSET, _y, BULLET_HITBOX_WIDTH, BULLET_HITBOX_HEIGHT};
}

float Bullet::get_x() const noexcept
{
    return _x;
}
float Bullet::get_y() const noexcept
{
    return _y;
}
const std::array<float, 3> &Bullet::get_previous_x() const noexcept
{
    return _previous_x;
}
const std::array<float, 3> &Bullet::get_previous_y() const noexcept
{
    return _previous_y;
}
