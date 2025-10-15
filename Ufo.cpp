#include <random>
#include <raylib.h>

#include "Animation.hpp"
#include "Global.hpp"
#include "Ufo.hpp"

constexpr float UFO_EXPLOSION_Y_OFFSET = 0.5F;
constexpr float UFO_EXPLOSION_MAGIC_OFFSET = 0.5F;

Ufo::Ufo(std::mt19937_64& i_random_engine) :
	_y(BASE_SIZE),
	_powerup_distribution(0, POWERUP_TYPES - 1),
	_timer_distribution(UFO_TIMER_MIN, UFO_TIMER_MAX),
	_animation(UFO_ANIMATION_SPEED, 2 * BASE_SIZE, "Resources/Images/Ufo.png"),
	_explosion(EXPLOSION_ANIMATION_SPEED, 2 * BASE_SIZE, "Resources/Images/ExplosionBig.png")
{
	reset(true, i_random_engine);
	_ufoappearsound = raylib::WaveSound("Resources/Sounds/UFO Enter.wav");
	for (GameTypes::Count puType = 0; puType < POWERUP_TYPES; puType++)
	{
		// Use implicit conversion to create the string
		std::string powerupIndex = std::to_string(puType);
		_powerup_animations.emplace_back(POWERUP_ANIMATION_SPEED, BASE_SIZE, "Resources/Images/Powerup" + powerupIndex + ".png");
	}
}

bool Ufo::check_bullet_collision(std::mt19937_64& i_random_engine, const Rectangle& i_bullet_hitbox)
{
	if (!_dead)
	{
		if (CheckCollisionRecs(get_hitbox(), i_bullet_hitbox))
		{			_dead = true;
			_ufoappearsound.Stop();			_explosion_x = _x;

			// Get the powerup type from the distribution and truncate to unsigned char range
			// This avoids the need for a static_cast
			GameTypes::Count powerupType = _powerup_distribution(i_random_engine) % POWERUP_TYPES;
			_powerups.emplace_back(_x + 0.5F * BASE_SIZE, _y, powerupType);

			return true;
		}
	}
	
	return false;
}

GameTypes::Count Ufo::check_powerup_collision(const Rectangle& i_player_hitbox) noexcept
{
	for (PowerUpItem& powerup : _powerups)
	{
		if (!powerup.isdead() && CheckCollisionRecs(powerup.get_hitbox(), i_player_hitbox))
		{
			powerup.isdead(true);

			//Plus 1, because 0 means we didn't pick up any powerups.
			return 1 + powerup.get_type();
		}
	}
	
	return 0;
}

void Ufo::draw(raylib::DrawSession& ds)
{
	if (!_dead)
	{
		_animation.draw(ds, _x, _y, WHITE);
	}
	else if (!_dead_animation_over)
	{
		// Only draw the explosion animation until it's finished
		_explosion.draw(ds, _explosion_x, _y - (UFO_EXPLOSION_Y_OFFSET * BASE_SIZE), Color(255, 36, 0, 255));
		_explosion.draw(ds, _explosion_x, (_y - (UFO_EXPLOSION_MAGIC_OFFSET * BASE_SIZE)), Color(255, 36, 0, 255));
		_explosion.draw(ds, _explosion_x, _y - 0.5F * BASE_SIZE, Color(255, 36, 0, 255));
	}

	for (const PowerUpItem& powerup : _powerups)
	{
		_powerup_animations[powerup.get_type()].draw(ds, powerup.getx(), powerup.gety(), WHITE);
	}
}

void Ufo::reset(bool i_dead, std::mt19937_64& i_random_engine)
{
	_dead = i_dead;
	_dead_animation_over = false;

	_explosion_x = SCREEN_WIDTH;
	_x = SCREEN_WIDTH;

	_timer = _timer_distribution(i_random_engine);

	_powerups.clear();
	_ufoappearsound.Stop();
	_animation.reset();
	_explosion.reset();
}

void Ufo::update(std::mt19937_64& i_random_engine)
{
	if (!_dead)
	{
		if (!_ufoappearsound.IsPlaying())
		{
			_ufoappearsound.Play();
		}

		_x -= UFO_MOVE_SPEED;

		//As soon as the UFO leaves the screen, it gets destroyed. But no powerups will appear.
		if (_x <= -2 * BASE_SIZE)
		{
			_dead = true;
			_ufoappearsound.Stop();
		}

		_animation.update();
	}
	else
	{
		if (!_dead_animation_over)
		{
			_dead_animation_over = _explosion.update();
		}

		if (_timer == 0)
		{
			reset(false, i_random_engine);
		}
		else
		{
			_timer--;
		}
	}
	for (PowerUpItem& powerup : _powerups)
	{
		//Why didn't I made an update function for the powerups?
		//No, seriously.
		//I did it for the Bullet struct.
		//But not for the PowerUpItem struct.
		//
		// powerup._y += POWERUP_SPEED;
		powerup.bump_y(POWERUP_SPEED);
		if (SCREEN_HEIGHT <= powerup.gety())
		{
			powerup.isdead(true);
		}
	}

	for (Animation& powerup_animation : _powerup_animations)
	{
		powerup_animation.update();
	}
	_powerups.erase(remove_if(_powerups.begin(), _powerups.end(), [](const PowerUpItem& i_powerup) noexcept
	{
		return i_powerup.isdead();
	}), _powerups.end());
}

Rectangle Ufo::get_hitbox() const noexcept
{
	return Rectangle(_x, _y, 2 * BASE_SIZE, BASE_SIZE);
}