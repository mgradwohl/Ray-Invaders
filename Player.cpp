// Corresponding header
#include "Player.hpp"

// Standard library headers
#include <chrono>
#include <random>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Animation.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "RLDrawSession.h"
#include "RLWindow.h"
#include "Ufo.hpp"

Player::Player() :
	_explosion(GlobalConstant::Int::EXPLOSION_ANIMATION_SPEED, GlobalConstant::Int::BASE_SIZE, "Resources/Images/Explosion.png")
{
	reset();

	_bullet_sprite = ::LoadTexture("Resources/Images/PlayerBullet.png");
	_player_sprite = ::LoadTexture("Resources/Images/Player.png");
	_playerlasersound = LoadSound("Resources/Sounds/Player Laser.wav");
	_powerupsound = LoadSound("Resources/Sounds/Power Up.wav");
	_playerdestroysound = LoadSound("Resources/Sounds/Player Destroy.wav");
}

bool Player::get_dead() const noexcept
{
	return _dead;
}

bool Player::get_dead_animation_over() const noexcept
{
	return _dead_animation_over;
}

GameTypes::Count Player::get_current_power() const noexcept
{
	return _current_power;
}

GameTypes::Duration Player::get_power_timer() const noexcept
{
	return _power_timer;
}

float Player::get_y() const noexcept
{
	return _y;
}

//I don't know why, but this is funny.
void Player::die() noexcept
{
	_dead = true;
}

void Player::draw(raylib::DrawSession& ds)
{
	if (!_dead)
	{
		//sprite.setPosition(x, y);
		//sprite.setTextureRect(sf::IntRect(BASE_SIZE * current_power, 0, BASE_SIZE, BASE_SIZE));
		Vector2 dest{ _x, _y};
		// Using float constants to avoid static_cast
		const Rectangle source{			GlobalConstant::BASE_SIZE * _current_power, // _current_power is unsigned char, safe implicit conversion to float
			0.0f,
			GlobalConstant::BASE_SIZE,
			GlobalConstant::BASE_SIZE
		};
		ds.DrawTexture(_player_sprite, source, dest, WHITE);

		for (const Bullet& bullet : _bullets)
		{
			//bullet_sprite.setPosition(bullet.x, bullet.y);
			//i_window.draw(bullet_sprite);

			dest.x = bullet.get_x();
			dest.y = bullet.get_y();
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
	_dead = false;
	_dead_animation_over = false;
	_shield_animation_over = true;

	_current_power = 0;
	_reload_timer = 0;
	_power_timer = 0;
	_x = 0.5f * (GlobalConstant::SCREEN_WIDTH - GlobalConstant::BASE_SIZE);
	_y = GlobalConstant::SCREEN_HEIGHT - 2.0f * GlobalConstant::BASE_SIZE;

	_bullets.clear();

	_explosion.reset();
}

void Player::update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo)
{
	if (!_dead)
	{
		GameTypes::Count powerup_type;
		if (IsKeyDown(KEY_LEFT))
		{
			if (4 == _current_power)
			{
				//Mirrored controls power-DOWN!
				_x = std::min(
					GlobalConstant::PLAYER_MOVE_SPEED + _x,
					GlobalConstant::SCREEN_WIDTH - 2.0f * GlobalConstant::BASE_SIZE
				);
			}
			else
			{
				_x = std::max(
					_x - GlobalConstant::PLAYER_MOVE_SPEED,
					GlobalConstant::BASE_SIZE
				);
			}
		}

		if (IsKeyDown(KEY_RIGHT))
		{
			if (4 == _current_power)
			{
				//Mirrored controls power-DOWN!
				//I'm never gonna get tired of this joke.
				//NEVER!
				_x = std::max(
					_x - GlobalConstant::PLAYER_MOVE_SPEED,
					GlobalConstant::BASE_SIZE
				);
			}
			else			{
				_x = std::min(
					GlobalConstant::PLAYER_MOVE_SPEED + _x,
					GlobalConstant::SCREEN_WIDTH - 2.0f * GlobalConstant::BASE_SIZE
				);
			}
		}

		if (!_reload_timer)
		{
			if (IsKeyPressed(KEY_Z))
			{
				PlaySound(_playerlasersound);

				if (2 == _current_power)
				{
					_reload_timer = GlobalConstant::Int::FAST_RELOAD_DURATION;
				}
				else
				{				_reload_timer = GlobalConstant::Int::RELOAD_DURATION;
				}                // No need for conversions since Bullet constructor now accepts float params directly
                _bullets.emplace_back(0.0f, -GlobalConstant::PLAYER_BULLET_SPEED, _x, _y);

				if (3 == _current_power)
				{
					const float leftX = _x - 0.375f * GlobalConstant::BASE_SIZE;
					const float rightX = _x + 0.375f * GlobalConstant::BASE_SIZE;
					_bullets.emplace_back(0.0f, -GlobalConstant::PLAYER_BULLET_SPEED, leftX, _y);
					_bullets.emplace_back(0.0f, -GlobalConstant::PLAYER_BULLET_SPEED, rightX, _y);
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

					_shield_animation_over = false;
				}
				else
				{
					_dead = true;
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

			_power_timer = GlobalConstant::Int::POWERUP_DURATION;
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

				// Compute intersection center for a precise impact point (similar to Base)
				const Rectangle eHB = enemy.get_hitbox();
				const Rectangle bHB = bullet.get_hitbox();
				const float inter_x1 = std::max(eHB.x, bHB.x);
				const float inter_y1 = std::max(eHB.y, bHB.y);
				const float inter_x2 = std::min(eHB.x + eHB.width, bHB.x + bHB.width);
				const float inter_y2 = std::min(eHB.y + eHB.height, bHB.y + bHB.height);
				const float inter_w = std::max(0.0f, inter_x2 - inter_x1);
				const float inter_h = std::max(0.0f, inter_y2 - inter_y1);
				const float impact_world_x = inter_x1 + 0.5f * inter_w;
				const float impact_world_y = inter_y1 + 0.5f * inter_h;

				// Convert to enemy-local coordinates so marker sticks to sprite as it moves
				const float rel_x = impact_world_x - enemy.get_x();
				const float rel_y = impact_world_y - enemy.get_y();
				enemy.add_impact_marker(rel_x, rel_y);

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

Rectangle Player::get_hitbox() const noexcept
{
	// Use the fraction constants from the F namespace
	return Rectangle(
		_x + 0.125f * GlobalConstant::BASE_SIZE, 
		_y + 0.125f * GlobalConstant::BASE_SIZE, 
		GlobalConstant::THREE_QUARTERS * GlobalConstant::BASE_SIZE, 
		GlobalConstant::THREE_QUARTERS * GlobalConstant::BASE_SIZE
	);
}





