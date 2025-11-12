// Corresponding header
#include "Bullet.hpp"

// Third-party headers
#include <ranges>
#include <raylib.h>

// Project headers
#include "Global.hpp"

Bullet::Bullet(float i_step_x, float i_step_y, float i_x, float i_y) noexcept
    : _x(i_x), _y(i_y), _real_x(i_x), _real_y(i_y), _step_x(i_step_x), _step_y(i_step_y)
{
    _previous_x.fill(_x);
    _previous_y.fill(_y);
}

auto Bullet::IsDead() const noexcept -> bool
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
        _real_x += _step_x;
        _real_y += _step_y;

        // Shift previous positions left by one, discarding the oldest
        // Use std::ranges::views::drop for clarity and safety
        std::ranges::copy(_previous_x | std::ranges::views::drop(1), _previous_x.begin());
        std::ranges::copy(_previous_y | std::ranges::views::drop(1), _previous_y.begin());

        // Store the current position as the newest history entry
        _previous_x.back() = _x;
        _previous_y.back() = _y;

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

auto Bullet::get_hitbox() const noexcept -> Rectangle
{
    // Smaller hitboxes make the game so much better!
    return Rectangle{_x + BULLET_HITBOX_X_OFFSET, _y, BULLET_HITBOX_WIDTH, BULLET_HITBOX_HEIGHT};
}

auto Bullet::get_x() const noexcept -> float
{
    return _x;
}
auto Bullet::get_y() const noexcept -> float
{
    return _y;
}
auto Bullet::get_previous_x() const noexcept -> const std::array<float, 3> &
{
    return _previous_x;
}
auto Bullet::get_previous_y() const noexcept -> const std::array<float, 3> &
{
    return _previous_y;
}
