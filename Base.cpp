#include <raylib.h>
#include "Bullet.hpp"
#include "Base.hpp"

Base::Base(unsigned short x) noexcept
{
	_x = x;
	_y = SCREEN_HEIGHT - 4 - (BASE_SIZE * 3);
}

void Base::reset() noexcept
{
	_dead = false;
	_damage = 0;
}

void Base::update(std::vector<Bullet>& i_enemy_bullets, unsigned short framecount)
{
	if (_dead)
	{
		// can't collide with a dead Base
		return;
	}

	_frame = _damage;
	if (_frame >= framecount)
	{
		_frame = framecount;
		_dead = true;
	}

	for (Bullet& enemy_bullet : i_enemy_bullets)
	{
		if (CheckCollisionRecs(get_hitbox(), enemy_bullet.get_hitbox()))
		{
			_damage++;
			enemy_bullet.IsDead(true);

			break;
		}
	}
}

void Base::draw(raylib::DrawSession& ds, const Texture2D& sprite)
{
	const Vector2 dest{ _x, _y };
	const Rectangle source{ BASE_WIDTH * _frame, 0, BASE_WIDTH, BASE_SIZE };
	ds.DrawTexture(sprite, source, dest, WHITE);
}

[[nodiscard]] Rectangle Base::get_hitbox() const noexcept
{
	return Rectangle(_x, _y, BASE_WIDTH, BASE_SIZE);
}