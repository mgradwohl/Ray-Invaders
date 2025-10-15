#pragma once
#include <raylib.h>
#include "Global.hpp"

class PowerUpItem
{
public:
	PowerUpItem(float i_x, float i_y, unsigned char i_type) noexcept;

	//0 - Shield
	//1 - Fast reload
	//2 - 3 bullets
	//3 - Mirrored controls (power-DOWN! I'm so proud of this joke)
	//Damn, I should've used enums. That would've made the code more readable.

	void bump_y(unsigned short ybump) noexcept;
    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    [[nodiscard]] auto get_type() const noexcept -> unsigned char;

    [[nodiscard]] auto isdead() const noexcept -> bool;
    void isdead(bool dead) noexcept;
    [[nodiscard]] auto getx() const noexcept -> float;
    [[nodiscard]] auto gety() const noexcept -> float;

  private:
    float         _x{0.0F};
    float         _y{0.0F};
    bool _dead{false};
	unsigned char _type{0};
};
