#pragma once
#include <vector>
#include "Bullet.hpp"
#include <raylib.h>


class Enemy
{
public:
	enum class Direction : char { Left = -1, Down = 0, Right = 1 };
	enum class Type : unsigned char { Cyan = 0, Purple = 1, Green = 2 };
	Enemy(Type i_type, float i_x, float i_y, unsigned char health = 1) noexcept;

	[[nodiscard]] unsigned char get_health() const noexcept;
	unsigned char get_hit_timer() const noexcept;
	Type get_type() const noexcept;
	[[nodiscard]] float get_x() const noexcept;
	[[nodiscard]] float get_y() const noexcept;

	void hit() noexcept;
	void move();
	void shoot(std::vector<Bullet>& i_enemy_bullets);
	void update() noexcept;

	[[nodiscard]] Rectangle get_hitbox() const noexcept;
private:
	Direction _direction{Direction::Down};
	unsigned char _health{1};
	//The enemy will appear white for a few frames after being hit, so that the player knows about it.
	unsigned char _hit_timer{0};
	Type _type{Type::Cyan};
	float _x{0.0f};
	float _y{0.0f};
	Sound _enemylaser{};
};