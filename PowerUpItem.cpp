// Corresponding header
#include "PowerUpItem.hpp"

// Third-party headers
// Third-party
#include <raylib.h>

// Project headers
// Project
#include "Global.hpp"

PowerUpItem::PowerUpItem(float i_x, float i_y, Type i_type) noexcept
    : _x(i_x), _y(i_y), _type(i_type)
{
}

void PowerUpItem::bump_y(GameTypes::Distance ybump) noexcept { _y += ybump; }

[[nodiscard]] auto PowerUpItem::get_hitbox() const noexcept -> Rectangle
{
    constexpr float HITBOX_SCALE = 0.5f;
    constexpr float HITBOX_OFFSET = 0.25f;
    return Rectangle{_x + HITBOX_OFFSET * GlobalConstant::BASE_SIZE,
                     _y + HITBOX_OFFSET * GlobalConstant::BASE_SIZE,
                     HITBOX_SCALE * GlobalConstant::BASE_SIZE,
                     HITBOX_SCALE * GlobalConstant::BASE_SIZE};
}

PowerUpItem::Type PowerUpItem::get_type() const noexcept { return _type; }

bool PowerUpItem::isdead() const noexcept { return _dead; }

void PowerUpItem::isdead(bool dead) noexcept { _dead = dead; }

float PowerUpItem::get_x() const noexcept { return _x; }

float PowerUpItem::get_y() const noexcept { return _y; }
