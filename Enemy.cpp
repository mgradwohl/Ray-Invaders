#include <array>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cmath>

#include <raylib.h>

#include "Global.hpp"
#include "Enemy.hpp"

Enemy::Enemy(Type i_type, float i_x, float i_y, unsigned char health) noexcept :
    _direction(Direction::Down),
    _health(health),
    _hit_timer(0),
    _type(i_type),
    _x(i_x), // No cast needed with float parameters
    _y(i_y) // No cast needed with float parameters
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
        const float rightBoundary = F::SCREEN_WIDTH - 2.0f * F::BASE_SIZE;
        const float leftBoundary = F::BASE_SIZE;
        
        if ((_direction == Direction::Right && _x >= rightBoundary) ||
            (_direction == Direction::Left && _x <= leftBoundary))
        {
            _direction = Direction::Down;
            _y += F::ENEMY_MOVE_SPEED;
        }        else
        {
            // Direction enum is defined with underlying type char, which can be converted to float
            // We can avoid the static_cast by using a switch statement or direct mapping
            float moveOffset = 0.0f;
            
            switch (_direction) {
                case Direction::Left:
                    moveOffset = -F::ENEMY_MOVE_SPEED;
                    break;
                case Direction::Right:
                    moveOffset = F::ENEMY_MOVE_SPEED;
                    break;
                default: // Direction::Down
                    moveOffset = 0.0f;
                    break;
            }
            
            _x = std::clamp<float>(_x + moveOffset, leftBoundary, rightBoundary);
        }
    }
    else
    {        _y = std::min<float>(_y + F::ENEMY_MOVE_SPEED, F::BASE_SIZE * ceil(_y / F::BASE_SIZE));
        if (_y >= F::BASE_SIZE * ceil(_y / F::BASE_SIZE) - 0.1f) // Using approximate comparison for float
        {            // Calculate row using floor division with floats
            const float rowFloat = _y / F::BASE_SIZE;
            // This cast is necessary for the modulo operation which requires an integer
            const int rowInt = static_cast<int>(rowFloat);
            _direction = (rowInt % 2 == 0) ? Direction::Left : Direction::Right;
        }
    }
}

void Enemy::shoot(std::vector<Bullet>& i_enemy_bullets)
{
    // No need for conversions now
    switch (_type)
    {
        case Type::Cyan:
        {
            i_enemy_bullets.emplace_back(0.0f, F::ENEMY_BULLET_SPEED, _x, _y);
            break;
        }
        case Type::Purple:
        {
            i_enemy_bullets.emplace_back(0.125F * F::ENEMY_BULLET_SPEED, F::ENEMY_BULLET_SPEED, _x, _y);
            i_enemy_bullets.emplace_back(-0.125F * F::ENEMY_BULLET_SPEED, F::ENEMY_BULLET_SPEED, _x, _y);
            break;
        }
        case Type::Green:
        {
            i_enemy_bullets.emplace_back(0.0f, F::ENEMY_BULLET_SPEED, _x, _y);
            i_enemy_bullets.emplace_back(0.25F * F::ENEMY_BULLET_SPEED, F::ENEMY_BULLET_SPEED, _x, _y);
            i_enemy_bullets.emplace_back(-0.25F * F::ENEMY_BULLET_SPEED, F::ENEMY_BULLET_SPEED, _x, _y);
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
			// Decrement health directly when it's above 0
			if (_health > 0) {
				_health--;
			}
		}

		_hit_timer--;
	}
}

Rectangle Enemy::get_hitbox() const noexcept
{
	// The sprite width is 16 pixels but the hitbox should be 12 pixels wide, centered
	constexpr float hitbox_width = 12.0f;
	constexpr float x_offset = (F::BASE_SIZE - hitbox_width) * 0.5f; // Center the 12px hitbox within the 16px sprite
	return Rectangle(_x + x_offset, _y + 0.25f * F::BASE_SIZE, hitbox_width, 0.5f * F::BASE_SIZE);
}