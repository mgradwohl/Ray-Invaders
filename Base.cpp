#include <raylib.h>
#include "Bullet.hpp"
#include "Base.hpp"

Base::Base(unsigned short x)
{
	_x = x;
	_y = SCREEN_HEIGHT - 4 - (BASE_SIZE * 3);
}

Base::~Base()
{
}

void Base::reset()
{
	dead = false;
	damage = 0;
}

void Base::update(std::vector<Bullet>& i_enemy_bullets)
{
	if (dead)
	{
		// can't collide with a dead Base
		return;
	}

	frame = damage;
	if (frame >= 4)
	{
		frame = 5;
		dead = true;
	}

	for (Bullet& enemy_bullet : i_enemy_bullets)
	{
		if (CheckCollisionRecs(get_hitbox(), enemy_bullet.get_hitbox()))
		{
			damage++;
			enemy_bullet.IsDead(true);

			break;
		}
	}
}

void Base::draw(raylib::DrawSession& ds, Texture2D& sprite)
{
	Vector2 dest{ _x, _y };
	Rectangle source{ BASE_WIDTH * frame, 0, BASE_WIDTH, BASE_SIZE };
	ds.DrawTexture(sprite, source, dest, WHITE);
}

Rectangle Base::get_hitbox() const
{
	return Rectangle(_x, _y, BASE_WIDTH, BASE_SIZE);
}