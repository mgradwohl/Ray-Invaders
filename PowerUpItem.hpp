#pragma once
#include <raylib.h>

#include "Global.hpp"

class PowerUpItem
{
  public:
    enum class Type : GameTypes::Count
    {
        Shield = 0,
        FastReload = 1,
        ThreeBullets = 2,
        MirroredControls = 3
    };

    PowerUpItem(float i_x, float i_y, Type i_type) noexcept;

    void bump_y(GameTypes::Distance ybump) noexcept;
    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    [[nodiscard]] auto get_type() const noexcept -> Type;

    [[nodiscard]] auto isdead() const noexcept -> bool;
    void isdead(bool dead) noexcept;
    [[nodiscard]] auto get_x() const noexcept -> float;
    [[nodiscard]] auto get_y() const noexcept -> float;

  private:
    float _x{0.0F};
    float _y{0.0F};
    bool _dead{false};
    Type _type{Type::Shield};
};
