#include <array>
#include <chrono>
#include <random>
#include <raylib.h>

#include "Animation.hpp"
#include "Global.hpp"
#include "Ufo.hpp"

Ufo::Ufo(std::mt19937_64& i_random_engine) :
	y(BASE_SIZE),
	powerup_distribution(0, POWERUP_TYPES - 1),
	timer_distribution(UFO_TIMER_MIN, UFO_TIMER_MAX),
	animation(UFO_ANIMATION_SPEED, 2 * BASE_SIZE, "Resources/Images/Ufo.png"),
	explosion(EXPLOSION_ANIMATION_SPEED, 2 * BASE_SIZE, "Resources/Images/ExplosionBig.png")
{
	reset(1, i_random_engine);
	ufoappearsound = raylib::WaveSound("Resources/Sounds/UFO Enter.wav");

	for (unsigned char a = 0; a < POWERUP_TYPES; a++)
	{
		powerup_animations.emplace_back(POWERUP_ANIMATION_SPEED, BASE_SIZE, "Resources/Images/Powerup" + std::to_string(static_cast<unsigned short>(a)) + ".png");
	}
}

bool Ufo::check_bullet_collision(std::mt19937_64& i_random_engine, const Rectangle& i_bullet_hitbox)
{
	if (!dead)
	{
		if (CheckCollisionRecs(get_hitbox(), i_bullet_hitbox))
		{
			dead = true;
			ufoappearsound.Stop();

			explosion_x = x;

			powerups.emplace_back(x + 0.5f * BASE_SIZE, y, powerup_distribution(i_random_engine));

			return true;
		}
	}
	
	return false;
}

unsigned char Ufo::check_powerup_collision(const Rectangle& i_player_hitbox)
{
	for (Powerup& powerup : powerups)
	{
		if (!powerup.dead && CheckCollisionRecs(powerup.get_hitbox(), i_player_hitbox))
		{
			powerup.dead = 1;

			//Plus 1, because 0 means we didn't pick up any powerups.
			return 1 + powerup.type;
		}
	}
	
	return 0;
}

void Ufo::draw(raylib::DrawSession& ds)
{
	if (!dead)
	{
		animation.draw(ds, x, y);
	}

	if (!dead_animation_over)
	{
		explosion.draw(ds, explosion_x, y - 0.5f * BASE_SIZE, Color(255, 36, 0, 255));
	}

	for (Powerup& powerup : powerups)
	{
		powerup_animations[powerup.type].draw(ds, powerup.x, powerup.y);
	}
}

void Ufo::reset(bool i_dead, std::mt19937_64& i_random_engine)
{
	dead = i_dead;
	dead_animation_over = 0;

	explosion_x = SCREEN_WIDTH;
	x = SCREEN_WIDTH;

	timer = timer_distribution(i_random_engine);

	powerups.clear();
	ufoappearsound.Stop();
	animation.reset();
	explosion.reset();
}

void Ufo::update(std::mt19937_64& i_random_engine)
{
	if (!dead)
	{
		if (!ufoappearsound.IsPlaying())
		{
			ufoappearsound.Play();
		}

		x -= UFO_MOVE_SPEED;

		//As soon as the UFO leaves the screen, it gets destroyed. But no powerups will appear.
		if (x <= -2 * BASE_SIZE)
		{
			dead = 1;
			ufoappearsound.Stop();
		}

		animation.update();
	}
	else
	{
		if (!dead_animation_over)
		{
			dead_animation_over = explosion.update();
		}

		if (!timer)
		{
			reset(0, i_random_engine);
		}
		else
		{
			timer--;
		}
	}

	for (Powerup& powerup : powerups)
	{
		//Why didn't I made an update function for the powerups?
		//No, seriously.
		//I did it for the Bullet struct.
		//But not for the Powerup struct.
		powerup.y += POWERUP_SPEED;

		if (SCREEN_HEIGHT <= powerup.y)
		{
			powerup.dead = 1;
		}
	}

	for (Animation& powerup_animation : powerup_animations)
	{
		powerup_animation.update();
	}

	powerups.erase(remove_if(powerups.begin(), powerups.end(), [](const Powerup& i_powerup)
	{
		return true == i_powerup.dead;
	}), powerups.end());
}

Rectangle Ufo::get_hitbox() const
{
	return Rectangle(x, y, 2 * BASE_SIZE, BASE_SIZE);
}