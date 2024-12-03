#include <raylib.h>
#include "Global.hpp"
#include "Bullet.hpp"

Bullet::Bullet(float i_step_x, float i_step_y, short i_x, short i_y) :
	_dead(false),
	_real_x(i_x),
	_real_y(i_y),
	_step_x(i_step_x),
	_x(i_x),
	_step_y(i_step_y),
	_y(i_y)
{
	_previous_x.fill(_x);
	_previous_y.fill(_y);
}

void Bullet::update() noexcept
{
	if (!_dead)
	{
		//I hate using floats, so we'll change real_x and _y and work only with integer values.
		_real_x += _step_x;
		_real_y += _step_y;

		for (unsigned char a = 0; a < _previous_x.size() - 1; a++)
		{
			_previous_x[a] = _previous_x[1 + a];
			_previous_y[a] = _previous_y[1 + a];
		}

		_previous_x[_previous_x.size() - 1] = _x;
		_previous_y[_previous_y.size() - 1] = _y;

		_x = round(_real_x);
		_y = round(_real_y);

		if (_x <= -BASE_SIZE || _y <= -BASE_SIZE || SCREEN_HEIGHT <= _y || SCREEN_WIDTH <= _x)
		{
			_dead = true;
		}
	}
}

Rectangle Bullet::get_hitbox() const noexcept
{
	//Smaller hitboxes make the game so much better!
	return Rectangle(_x + 0.375F * BASE_SIZE, _y + 0.375F * BASE_SIZE, 0.25F * BASE_SIZE, 0.25F * BASE_SIZE);
}

bool Bullet::IsDead() const noexcept
{
	return _dead;
};

void Bullet::IsDead(bool d) noexcept
{
	_dead = d;
}
