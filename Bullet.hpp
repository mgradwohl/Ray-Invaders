#pragma once
#include <array>
#include <raylib.h>

class Bullet
{
public:
	Bullet(float i_step_x, float i_step_y, short i_x, short i_y);
	[[nodiscard]] const bool IsDead() const noexcept;
	void IsDead(bool dead) noexcept;
	void update() noexcept;
	[[nodiscard]] Rectangle get_hitbox() const noexcept;

	// TODO why are these public
	//We also use this struct in the Player class. But the player's bullets don't have a tail. So it may seem a bit redundant. But I'm too lazy to fix it.
	std::array<float, 3> _previous_x;
	std::array<float, 3> _previous_y;

	float _x;
	float _y;

private:
	bool _dead;

	float _real_x;
	float _real_y;
	float _step_x;
	float _step_y;
};
