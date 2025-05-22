#include <array>
#include <chrono>
#include <algorithm>
#include <vector>

#include <raylib.h>

#include "Global.hpp"
#include "Enemy.hpp"

Enemy::Enemy(Type i_type, unsigned short i_x, unsigned short i_y, unsigned char health) noexcept :
    _direction(Direction::Down),
    _health(health),
    _hit_timer(0),
    _type(i_type),
    _x(static_cast<float>(i_x)),
    _y(static_cast<float>(i_y))
{
    _enemylaser = LoadSound("Resources/Sounds/Enemy Laser.wav");
}

unsigned char Enemy::get_health() const noexcept { return _health; }
unsigned char Enemy::get_hit_timer() const noexcept { return _hit_timer; }
[[nodiscard]] Enemy::Type Enemy::get_type() const noexcept { return _type; }
float Enemy::get_x() const noexcept { return _x; }
float Enemy::get_y() const noexcept { return _y; }

void Enemy::hit() noexcept
{ 
	_hit_timer = ENEMY_HIT_TIMER_DURATION;
}

void Enemy::move()
{
    if (_direction != Direction::Down)
    {
        if ((_direction == Direction::Right && _x == static_cast<float>(SCREEN_WIDTH - 2 * BASE_SIZE)) ||
            (_direction == Direction::Left && _x == static_cast<float>(BASE_SIZE)))
        {
            _direction = Direction::Down;
            _y += static_cast<float>(ENEMY_MOVE_SPEED);
        }
        else
        {
            _x = std::clamp<float>(_x + static_cast<float>(ENEMY_MOVE_SPEED) * static_cast<int>(_direction), static_cast<float>(BASE_SIZE), static_cast<float>(SCREEN_WIDTH - 2 * BASE_SIZE));
        }
    }
    else
    {
        _y = std::min<float>(_y + static_cast<float>(ENEMY_MOVE_SPEED), static_cast<float>(BASE_SIZE * ceil(_y / static_cast<float>(BASE_SIZE))));
        if (_y == static_cast<float>(BASE_SIZE * ceil(_y / static_cast<float>(BASE_SIZE))))
        {
            _direction = (0 == (static_cast<int>(_y) / BASE_SIZE) % 2) ? Direction::Left : Direction::Right;
        }
    }
}

void Enemy::shoot(std::vector<Bullet>& i_enemy_bullets)
{
    switch (_type)
    {
        case Type::Cyan:
        {
            i_enemy_bullets.emplace_back(0.0f, static_cast<float>(ENEMY_BULLET_SPEED), static_cast<short>(_x), static_cast<short>(_y));
            break;
        }
        case Type::Purple:
        {
            i_enemy_bullets.emplace_back(0.125F * static_cast<float>(ENEMY_BULLET_SPEED), static_cast<float>(ENEMY_BULLET_SPEED), static_cast<short>(_x), static_cast<short>(_y));
            i_enemy_bullets.emplace_back(-0.125F * static_cast<float>(ENEMY_BULLET_SPEED), static_cast<float>(ENEMY_BULLET_SPEED), static_cast<short>(_x), static_cast<short>(_y));
            break;
        }
        case Type::Green:
        {
            i_enemy_bullets.emplace_back(0.0f, static_cast<float>(ENEMY_BULLET_SPEED), static_cast<short>(_x), static_cast<short>(_y));
            i_enemy_bullets.emplace_back(0.25F * static_cast<float>(ENEMY_BULLET_SPEED), static_cast<float>(ENEMY_BULLET_SPEED), static_cast<short>(_x), static_cast<short>(_y));
            i_enemy_bullets.emplace_back(-0.25F * static_cast<float>(ENEMY_BULLET_SPEED), static_cast<float>(ENEMY_BULLET_SPEED), static_cast<short>(_x), static_cast<short>(_y));
            break;
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
			int new_health = std::max(0, static_cast<int>(_health) - 1);
			_health = static_cast<unsigned char>(new_health);
		}

		_hit_timer--;
	}
}

Rectangle Enemy::get_hitbox() const noexcept
{
	return Rectangle(_x + 0.25f * BASE_SIZE, _y + 0.25f * BASE_SIZE, 0.5f * BASE_SIZE, 0.5f * BASE_SIZE);
}