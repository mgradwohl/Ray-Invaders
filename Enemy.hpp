#pragma once
#include "Bullet.hpp"


class Enemy
{
public:
	Enemy(unsigned char i_type, unsigned short i_x, unsigned short i_y);

	unsigned char get_health() const;
	unsigned char get_hit_timer() const;
	unsigned char get_type() const;

	float get_x() const;
	float get_y() const;

	void hit();
	void move();
	void shoot(std::vector<Bullet>& i_enemy_bullets);
	void update();

	Rectangle get_hitbox() const;
private:
	//-1 - Left
	//0 - Down
	//1 - Right
	char _direction;

	unsigned char _health;
	//The enemy will appear white for a few frames after being hit, so that the player knows about it.
	unsigned char _hit_timer;
	//0 - Cyan one
	//1 - Purple one
	//2 - Green one
	unsigned char _type;

	float _x;
	float _y;
	Sound _enemylaser;
};