#include <array>
#include <chrono>
#include <random>
#include <raylib.h>
#include "RLWindow.h"
#include "RLDrawSession.h"

#include "Animation.hpp"
#include "Global.hpp"
#include "Enemy.hpp"
#include "EnemyManager.hpp"
#include "Bullet.hpp"
EnemyManager::EnemyManager() noexcept :
	_shoot_distribution(0, ENEMY_SHOOT_CHANCE)
{
	//We have a function that sets everything to the initial state, so why not use it?
	reset(0);

	_enemy_bullet_sprite = ::LoadTexture("Resources/Images/EnemyBullet.png");

	for (unsigned char a = 0; a < ENEMY_TYPES; a++)
	{
		_enemy_animations.emplace_back(1 + _move_pause, BASE_SIZE, "Resources/Images/Enemy" + std::to_string(static_cast<unsigned short>(a)) + ".png");
	}
	_enemymove = LoadSound("Resources/Sounds/Enemy Move.wav");
	_enemydestroy = LoadSound("Resources/Sounds/Enemy Destroy.wav");
}

bool EnemyManager::reached_player(unsigned short i_player_y) const
{
	for (const Enemy& enemy : _enemies)
	{
		if (enemy.get_y() > i_player_y - 0.5F * BASE_SIZE)
		{
			//As soon as the enemies reach the player, the game is over!
			return true;
		}
	}

	return false;
}

void EnemyManager::draw(raylib::DrawSession& ds)
{
	for (const Bullet& bullet : _enemy_bullets)
	{
		//Drawing the tail of the bullet.
		for (unsigned char a = 0; a < bullet._previous_x.size(); a++)
		{
			//enemy_bullet_sprite.setPosition(bullet.previous_x[a], bullet.previous_y[a]);
			//enemy_bullet_sprite.setTextureRect(sf::IntRect(BASE_SIZE * a, 0, BASE_SIZE, BASE_SIZE));

			//i_window.draw(enemy_bullet_sprite);

			const Vector2 dest{ bullet._previous_x[a], bullet._previous_y[a]};
			const Rectangle source{ BASE_SIZE * a, 0, BASE_SIZE, BASE_SIZE };
			ds.DrawTexture(_enemy_bullet_sprite, source, dest, WHITE);
		}

		//Drawing the bullet itself.
		//enemy_bullet_sprite.setPosition(bullet.x, bullet.y);
		//enemy_bullet_sprite.setTextureRect(sf::IntRect(BASE_SIZE * bullet.previous_x.size(), 0, BASE_SIZE, BASE_SIZE));

		//i_window.draw(enemy_bullet_sprite);

		const Vector2 dest{ bullet._x, bullet._y};
		const Rectangle source{ BASE_SIZE * bullet._previous_x.size(), 0, BASE_SIZE, BASE_SIZE };
		ds.DrawTexture(_enemy_bullet_sprite, source, dest, WHITE);
	}

	for (const Enemy& enemy : _enemies)
	{
		//When the enemy gets hit, it's gonna appear white.
		Color enemy_color = WHITE;

		if (!enemy.get_hit_timer())
		{
			//Otherwise, we're gonna color it.
			switch (enemy.get_type())
			{
				case 0:
				{
					enemy_color = Color{ 0, 255, 255, 255 }; //CYAN

					break;
				}
				case 1:
				{
					enemy_color = PURPLE;

					break;
				}
				case 2:
				{
					enemy_color = GREEN;
				}
			}
		}
		else
		{
			PlaySound(_enemydestroy);
		}

		_enemy_animations[enemy.get_type()].draw(ds, enemy.get_x(), enemy.get_y(), enemy_color);
	}
}

void EnemyManager::reset(unsigned short i_level)
{
	//RESET EVERYTHING!!!!
	unsigned char enemy_x = 0;
	unsigned char enemy_y = 0;

	std::string level_sketch = "";

	_move_pause = std::max<short>(ENEMY_MOVE_PAUSE_START_MIN, ENEMY_MOVE_PAUSE_START - ENEMY_MOVE_PAUSE_DECREASE * i_level);
	_move_timer = _move_pause;

	_shoot_distribution = std::uniform_int_distribution<unsigned short>(0, std::max<short>(ENEMY_SHOOT_CHANCE_MIN, ENEMY_SHOOT_CHANCE - ENEMY_SHOOT_CHANCE_INCREASE * i_level));

	for (Animation& enemy_animation : _enemy_animations)
	{
		enemy_animation.reset();
	}

	_enemy_bullets.clear();

	_enemies.clear();

	//There are 8 levels. Once the player finishes level 8, we go back to level 4. This is the same thing we did in the game "Frogger".
	//Go watch that video, btw!
	if (TOTAL_LEVELS <= i_level)
	{
		i_level = 0.5F * TOTAL_LEVELS + i_level % static_cast<unsigned char>(0.5F * TOTAL_LEVELS);
	}

	//Here you can see my pro level design skills!
	switch (i_level)
	{
		case 0:
		{
			level_sketch = "0 0 0 0 0 0 0 0 \n 0 0 0 0 0 0 0 0\n0 0 0 0 0 0 0 0 \n 0 0 0 0 0 0 0 0";

			break;
		}
		case 1:
		{
			level_sketch = "0000000000000000\n0000000000000000\n0000000000000000\n0000000000000000";

			break;
		}
		case 2:
		{
			//OH MY GOD, IS THAT A CHECKBOARD PATTERN?!
			level_sketch = "1010101010101010\n0101010101010101\n1010101010101010\n0101010101010101";

			break;
		}
		case 3:
		{
			level_sketch = "1111111111111111\n1111111111111111\n1111111111111111\n1111111111111111";

			break;
		}
		case 4:
		{
			//CHECKBOARD PATTERN WITH A SOLID PATTERN! I'M A GENIUS!
			level_sketch = "2222222222222222\n1111111111111111\n1010101010101010\n0101010101010101";

			break;
		}
		case 5:
		{
			//EASY ENEMIES AT THE TOP AND HARD ENEMIES AT THE BOTTOM! IT'S A REVOLUTION IN LEVEL DESIGN!
			level_sketch = "0000000000000000\n2222222222222222\n1111111111111111\n1111111111111111";

			break;
		}
		case 6:
		{
			//CHECKBOARD PATTERN AGAIN?!
			level_sketch = "2121212121212121\n1212121212121212\n2121212121212121\n1212121212121212";

			break;
		}
		case 7:
		{
			level_sketch = "2222222222222222\n2222222222222222\n2222222222222222\n2222222222222222";
		}
	}

	//Here we're converting each character into an enemy.
	for (const char sketch_character : level_sketch)
	{
		enemy_x++;

		switch (sketch_character)
		{
			case '\n':
			{
				enemy_x = 0;
				enemy_y++;

				break;
			}
			case '0':
			{
				_enemies.emplace_back(0, BASE_SIZE * (1 + enemy_x), BASE_SIZE * (2 + enemy_y));

				break;
			}
			case '1':
			{
				_enemies.emplace_back(1, BASE_SIZE * (1 + enemy_x), BASE_SIZE * (2 + enemy_y));

				break;
			}
			case '2':
			{
				_enemies.emplace_back(2, BASE_SIZE * (1 + enemy_x), BASE_SIZE * (2 + enemy_y));
			}
		}
	}
}

void EnemyManager::update(std::mt19937_64& i_random_engine)
{
	std::vector<Enemy>::iterator dead_enemies_start;

	if (!_move_timer)
	{
		_move_timer = _move_pause;

		for (Enemy& enemy : _enemies)
		{
			enemy.move();
		}

		for (Animation& enemy_animation : _enemy_animations)
		{
			//The enemies change their frame after each move.
			enemy_animation.change_current_frame();
		}
		PlaySound(_enemymove);
	}
	else
	{
		_move_timer--;
	}

	for (Enemy& enemy : _enemies)
	{
		enemy.update();

		if (!_shoot_distribution(i_random_engine))
		{
			enemy.shoot(_enemy_bullets);
		}
	}

	//I used a lambda!
	//I'm a pro!
	//No, not like that.
	//I'M A PROFESSIONAL C++ PROGRAMMER!!!!
	//Yeah, that's better.
	dead_enemies_start = remove_if(_enemies.begin(), _enemies.end(), [](const Enemy& i_enemy)
	{
		return 0 == i_enemy.get_health();
	});

	//The more enemies we kill, the faster they become.
	_move_pause = std::max<int>(ENEMY_MOVE_PAUSE_MIN, _move_pause - ENEMY_MOVE_PAUSE_DECREASE * (_enemies.end() - dead_enemies_start));

	_enemies.erase(dead_enemies_start, _enemies.end());

	for (Bullet& enemy_bullet : _enemy_bullets)
	{
		enemy_bullet.update();
	}

	//I used a lambda!
	//AGAIN!
	_enemy_bullets.erase(remove_if(_enemy_bullets.begin(), _enemy_bullets.end(), [](const Bullet& i_bullet)
	{
		return 1 == i_bullet.     IsDead();
	}), _enemy_bullets.end());
}

//Yes, that's a reference return type.
std::vector<Bullet>& EnemyManager::get_enemy_bullets() noexcept
{
	return _enemy_bullets;
}

std::vector<Enemy>& EnemyManager::get_enemies() noexcept
{
	return _enemies;
}