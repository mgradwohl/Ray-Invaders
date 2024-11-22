#pragma once
class Bullet
{

private:
	bool dead;

	float real_x;
	float real_y;
	float step_x;
	float step_y;


public:
	Bullet(float i_step_x, float i_step_y, short i_x, short i_y);
	const bool IsDead() const;
	void IsDead(bool dead);
	void update();
	Rectangle get_hitbox() const;

	// TODO why are these public
	//We also use this struct in the Player class. But the player's bullets don't have a tail. So it may seem a bit redundant. But I'm too lazy to fix it.
	std::array<short, 3> previous_x;
	std::array<short, 3> previous_y;

	short x;
	short y;

};
