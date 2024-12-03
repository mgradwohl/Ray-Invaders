#include <array>
#include <chrono>
#include <algorithm>
#include <vector>

#include <raylib.h>

#include "Global.hpp"
#include "Enemy.hpp"

Enemy::Enemy(unsigned char i_type, unsigned short i_x, unsigned short i_y) noexcept :
	_direction(0 == (i_y / BASE_SIZE) % 2 ? -1 : 1),
	_health(1 + i_type),
	_hit_timer(0),
	_type(i_type),
	_x(i_x),
	_y(i_y)
{
	_enemylaser = LoadSound("Resources/Sounds/Enemy Laser.wav");
}

unsigned char Enemy::get_health() const noexcept
{
	return _health;
}

unsigned char Enemy::get_hit_timer() const noexcept
{
	return _hit_timer;
}

unsigned char Enemy::get_type() const noexcept
{
	return _type;
}

float Enemy::get_x() const noexcept
{
	return _x;
}

float Enemy::get_y() const noexcept
{
	return _y;
}

void Enemy::hit() noexcept
{ 
	_hit_timer = ENEMY_HIT_TIMER_DURATION;
}

void Enemy::move()
{
	if (0 != _direction)
	{
		if ((1 == _direction && _x == SCREEN_WIDTH - 2 * BASE_SIZE) || (-1 == _direction && _x == BASE_SIZE))
		{
			//Once we reach the edge, we start moving down until we reach the next row.
			_direction = 0;

			_y += ENEMY_MOVE_SPEED;
		}
		else
		{
			//Moving horizontally.
			_x = std::clamp<short>(_x + ENEMY_MOVE_SPEED * _direction, BASE_SIZE, SCREEN_WIDTH - 2 * BASE_SIZE);
		}
	}
	else
	{
		_y = std::min<short>(_y + ENEMY_MOVE_SPEED, BASE_SIZE * ceil(_y / static_cast<float>(BASE_SIZE)));

		if (_y == BASE_SIZE * ceil(_y / static_cast<float>(BASE_SIZE)))
		{
			//Moving in a snake pattern. We use the modulo operator to decide whether to move left or right.
			_direction = 0 == (((int)_y) / BASE_SIZE) % 2 ? -1 : 1;
		}
	}
}

void Enemy::shoot(std::vector<Bullet>& i_enemy_bullets)
{
	switch (_type)
	{
		case 0:
		{
			i_enemy_bullets.emplace_back(0, ENEMY_BULLET_SPEED, _x, _y);

			break;
		}
		case 1:
		{
			i_enemy_bullets.emplace_back(0.125F * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, _x, _y);
			i_enemy_bullets.emplace_back(-0.125F * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, _x, _y);

			break;
		}
		case 2:
		{
			i_enemy_bullets.emplace_back(0, ENEMY_BULLET_SPEED, _x, _y);
			i_enemy_bullets.emplace_back(0.25F * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, _x, _y);
			i_enemy_bullets.emplace_back(-0.25F * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, _x, _y);
		}
	}
	PlaySound(_enemylaser);
}

void Enemy::update() noexcept
{
	if (0 < _hit_timer)
	{
		if (1 == _hit_timer)
		{
			_health = std::max(0, _health - 1);
		}

		_hit_timer--;
	}
}

Rectangle Enemy::get_hitbox() const noexcept
{
	return Rectangle(_x + 0.25F * BASE_SIZE, _y + 0.25F * BASE_SIZE, 0.5F * BASE_SIZE, 0.5F * BASE_SIZE);
}