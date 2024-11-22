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
	explosion(EXPLOSION_ANIMATION_SPEED, BASE_SIZE, "Resources/Images/Explosion.png")
{
	reset();

	bullet_sprite = ::LoadTexture("Resources/Images/PlayerBullet.png");
	player_sprite = ::LoadTexture("Resources/Images/Player.png");
	playerlaser = LoadSound("Resources/Sounds/Player Laser.wav");
	powerup = LoadSound("Resources/Sounds/Power Up.wav");
	playerdestroy = LoadSound("Resources/Sounds/Player Destroy.wav");
}

bool Player::get_dead() const
{
	return dead;
}

bool Player::get_dead_animation_over() const
{
	return dead_animation_over;
}

unsigned char Player::get_current_power() const
{
	return current_power;
}

unsigned short Player::get_power_timer() const
{
	return power_timer;
}

unsigned short Player::get_y() const
{
	return y;
}

//I don't know why, but this is funny.
void Player::die()
{
	dead = 1;
}

void Player::draw(raylib::DrawSession& ds)
{
	if (!dead)
	{
		//sprite.setPosition(x, y);
		//sprite.setTextureRect(sf::IntRect(BASE_SIZE * current_power, 0, BASE_SIZE, BASE_SIZE));

		Vector2 dest{ x, y};
		Rectangle source{ BASE_SIZE * current_power, 0, BASE_SIZE, BASE_SIZE };
		ds.DrawTexture(player_sprite, source, dest, WHITE);

		for (const Bullet& bullet : bullets)
		{
			//bullet_sprite.setPosition(bullet.x, bullet.y);
			//i_window.draw(bullet_sprite);

			dest.x = bullet.x;
			dest.y = bullet.y;
			ds.DrawTexture(bullet_sprite, source, dest, WHITE);
		}
		//i_window.draw(sprite);

		if (!shield_animation_over)
		{
			//Once we get hit while having a shield, the shield will be destroyed. We'll show a blue explosion.
			explosion.draw(ds, x, y, Color(0, 109, 255, 255));
		}
	}
	else if (!dead_animation_over)
	{
		explosion.draw(ds, x, y, Color(255, 36, 0, 255));
	}
}

void Player::reset()
{
	dead = 0;
	dead_animation_over = 0;
	shield_animation_over = 1;

	current_power = 0;
	reload_timer = 0;

	power_timer = 0;
	x = 0.5f * (SCREEN_WIDTH - BASE_SIZE);
	y = SCREEN_HEIGHT - 2 * BASE_SIZE;

	bullets.clear();

	explosion.reset();
}

void Player::update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo)
{
	if (!dead)
	{
		unsigned char powerup_type;

		if (IsKeyDown(KEY_LEFT))
		{
			if (4 == current_power)
			{
				//Mirrored controls power-DOWN!
				x = std::min<int>(PLAYER_MOVE_SPEED + x, SCREEN_WIDTH - 2 * BASE_SIZE);
			}
			else
			{
				x = std::max<int>(x - PLAYER_MOVE_SPEED, BASE_SIZE);
			}
		}

		if (IsKeyDown(KEY_RIGHT))
		{
			if (4 == current_power)
			{
				//Mirrored controls power-DOWN!
				//I'm never gonna get tired of this joke.
				//NEVER!
				x = std::max<int>(x - PLAYER_MOVE_SPEED, BASE_SIZE);
			}
			else
			{
				x = std::min<int>(PLAYER_MOVE_SPEED + x, SCREEN_WIDTH - 2 * BASE_SIZE);
			}
		}

		if (!reload_timer)
		{
			if (IsKeyPressed(KEY_Z))
			{
				PlaySound(playerlaser);

				if (2 == current_power)
				{
					reload_timer = FAST_RELOAD_DURATION;
				}
				else
				{
					reload_timer = RELOAD_DURATION;
				}

				bullets.emplace_back(0, -PLAYER_BULLET_SPEED, x, y);

				if (3 == current_power)
				{
					bullets.emplace_back(0, -PLAYER_BULLET_SPEED, x - 0.375f * BASE_SIZE, y);
					bullets.emplace_back(0, -PLAYER_BULLET_SPEED, x + 0.375f * BASE_SIZE, y);
				}
			}
		}
		else
		{
			reload_timer--;
		}

		for (Bullet& enemy_bullet : i_enemy_bullets)
		{
			if (CheckCollisionRecs( get_hitbox(), enemy_bullet.get_hitbox()))
			{
				if (1 == current_power)
				{
					current_power = 0;

					shield_animation_over = 0;
				}
				else
				{
					dead = 1;
					PlaySound(playerdestroy);
				}

				enemy_bullet.IsDead(true);

				break;
			}
		}

		powerup_type = i_ufo.check_powerup_collision(get_hitbox());

		if (0 < powerup_type)
		{
			current_power = powerup_type;

			power_timer = POWERUP_DURATION;
			PlaySound(powerup);
		}

		if (!power_timer)
		{
			current_power = 0;
		}
		else
		{
			power_timer--;
		}

		if (!shield_animation_over)
		{
			shield_animation_over = explosion.update();
		}
	}
	else if (!dead_animation_over)
	{
		dead_animation_over = explosion.update();
	}

	for (Bullet& bullet : bullets)
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
		for (Bullet& bullet : bullets)
		{
			if (!bullet.IsDead() && 0 < enemy.get_health() && CheckCollisionRecs(enemy.get_hitbox(), bullet.get_hitbox()))
			{
				bullet.IsDead(true);

				enemy.hit();

				break;
			}
		}
	}

	bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet& i_bullet)
	{
		return true == i_bullet.IsDead();
	}), bullets.end());
}

Rectangle Player::get_hitbox() const
{
	return Rectangle(x + 0.125f * BASE_SIZE, y + 0.125f * BASE_SIZE, 0.75f * BASE_SIZE, 0.75f * BASE_SIZE);
}