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
	[[nodiscard]] Rectangle get_hitbox() const noexcept;
	unsigned char get_type() const noexcept;

	bool isdead() const noexcept;
	void isdead(bool dead) noexcept;
	float getx() const noexcept;
	float gety() const noexcept;

private:
	float _x;
	float _y;
	bool _dead;
	unsigned char _type;
};
