#include <array>
#include <chrono>
#include <random>
#include <raylib.h>

#include "RLWindow.h"
#include "RLDrawSession.h"

#include "Animation.hpp"
#include "Global.hpp"
#include "Enemy.hpp"
#include "Ufo.hpp"
#include "Player.hpp"

Player::Player() :
	_explosion(EXPLOSION_ANIMATION_SPEED, BASE_SIZE, "Resources/Images/Explosion.png")
{
	reset();

	_bullet_sprite = ::LoadTexture("Resources/Images/PlayerBullet.png");
	_player_sprite = ::LoadTexture("Resources/Images/Player.png");
	_playerlasersound = LoadSound("Resources/Sounds/Player Laser.wav");
	_powerupsound = LoadSound("Resources/Sounds/Power Up.wav");
	_playerdestroysound = LoadSound("Resources/Sounds/Player Destroy.wav");
}

bool Player::get_dead() const
{
	return _dead;
}

bool Player::get_dead_animation_over() const
{
	return _dead_animation_over;
}

unsigned char Player::get_current_power() const
{
	return _current_power;
}

unsigned short Player::get_power_timer() const
{
	return _power_timer;
}

float Player::get_y() const
{
	return _y;
}

//I don't know why, but this is funny.
void Player::die()
{
	_dead = 1;
}

void Player::draw(raylib::DrawSession& ds)
{
	if (!_dead)
	{
		//sprite.setPosition(x, y);
		//sprite.setTextureRect(sf::IntRect(BASE_SIZE * current_power, 0, BASE_SIZE, BASE_SIZE));

		Vector2 dest{ _x, _y};
		Rectangle source{ BASE_SIZE * _current_power, 0, BASE_SIZE, BASE_SIZE };
		ds.DrawTexture(_player_sprite, source, dest, WHITE);

		for (const Bullet& bullet : _bullets)
		{
			//bullet_sprite.setPosition(bullet.x, bullet.y);
			//i_window.draw(bullet_sprite);

			dest.x = bullet._x;
			dest.y = bullet._y;
			ds.DrawTexture(_bullet_sprite, source, dest, WHITE);
		}
		//i_window.draw(sprite);

		if (!_shield_animation_over)
		{
			//Once we get hit while having a shield, the shield will be destroyed. We'll show a blue explosion.
			_explosion.draw(ds, _x, _y, Color(0, 109, 255, 255));
		}
	}
	else if (!_dead_animation_over)
	{
		_explosion.draw(ds, _x, _y, Color(255, 36, 0, 255));
	}
}

void Player::reset()
{
	_dead = 0;
	_dead_animation_over = 0;
	_shield_animation_over = 1;

	_current_power = 0;
	_reload_timer = 0;

	_power_timer = 0;
	_x = 0.5f * (SCREEN_WIDTH - BASE_SIZE);
	_y = SCREEN_HEIGHT - 2 * BASE_SIZE;

	_bullets.clear();

	_explosion.reset();
}

void Player::update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo)
{
	if (!_dead)
	{
		unsigned char powerup_type;

		if (IsKeyDown(KEY_LEFT))
		{
			if (4 == _current_power)
			{
				//Mirrored controls power-DOWN!
				_x = std::min<int>(PLAYER_MOVE_SPEED + _x, SCREEN_WIDTH - 2 * BASE_SIZE);
			}
			else
			{
				_x = std::max<int>(_x - PLAYER_MOVE_SPEED, BASE_SIZE);
			}
		}

		if (IsKeyDown(KEY_RIGHT))
		{
			if (4 == _current_power)
			{
				//Mirrored controls power-DOWN!
				//I'm never gonna get tired of this joke.
				//NEVER!
				_x = std::max<int>(_x - PLAYER_MOVE_SPEED, BASE_SIZE);
			}
			else
			{
				_x = std::min<int>(PLAYER_MOVE_SPEED + _x, SCREEN_WIDTH - 2 * BASE_SIZE);
			}
		}

		if (!_reload_timer)
		{
			if (IsKeyPressed(KEY_Z))
			{
				PlaySound(_playerlasersound);

				if (2 == _current_power)
				{
					_reload_timer = FAST_RELOAD_DURATION;
				}
				else
				{
					_reload_timer = RELOAD_DURATION;
				}

				_bullets.emplace_back(0, -PLAYER_BULLET_SPEED, _x, _y);

				if (3 == _current_power)
				{
					_bullets.emplace_back(0, -PLAYER_BULLET_SPEED, _x - 0.375f * BASE_SIZE, _y);
					_bullets.emplace_back(0, -PLAYER_BULLET_SPEED, _x + 0.375f * BASE_SIZE, _y);
				}
			}
		}
		else
		{
			_reload_timer--;
		}

		for (Bullet& enemy_bullet : i_enemy_bullets)
		{
			if (CheckCollisionRecs( get_hitbox(), enemy_bullet.get_hitbox()))
			{
				if (1 == _current_power)
				{
					_current_power = 0;

					_shield_animation_over = 0;
				}
				else
				{
					_dead = 1;
					PlaySound(_playerdestroysound);
				}

				enemy_bullet.IsDead(true);

				break;
			}
		}

		powerup_type = i_ufo.check_powerup_collision(get_hitbox());

		if (0 < powerup_type)
		{
			_current_power = powerup_type;

			_power_timer = POWERUP_DURATION;
			PlaySound(_powerupsound);
		}

		if (!_power_timer)
		{
			_current_power = 0;
		}
		else
		{
			_power_timer--;
		}

		if (!_shield_animation_over)
		{
			_shield_animation_over = _explosion.update();
		}
	}
	else if (!_dead_animation_over)
	{
		_dead_animation_over = _explosion.update();
	}

	for (Bullet& bullet : _bullets)
	{
		bullet.update();
		
		if (!bullet.IsDead())
		{
			if (i_ufo.check_bullet_collision(i_random_engine, bullet.get_hitbox()))
			{
				bullet.IsDead(true);
			}
		}
	}

	for (Enemy& enemy : i_enemies)
	{
		for (Bullet& bullet : _bullets)
		{
			if (!bullet.IsDead() && 0 < enemy.get_health() && CheckCollisionRecs(enemy.get_hitbox(), bullet.get_hitbox()))
			{
				bullet.IsDead(true);

				enemy.hit();

				break;
			}
		}
	}

	_bullets.erase(remove_if(_bullets.begin(), _bullets.end(), [](const Bullet& i_bullet)
	{
		return true == i_bullet.IsDead();
	}), _bullets.end());
}

Rectangle Player::get_hitbox() const
{
	return Rectangle(_x + 0.125f * BASE_SIZE, _y + 0.125f * BASE_SIZE, 0.75f * BASE_SIZE, 0.75f * BASE_SIZE);
}