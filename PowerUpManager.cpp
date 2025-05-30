#include <raylib.h>
#include "RLDrawSession.h"
#include "Global.hpp"
#include "PowerUpManager.hpp"

PowerUpManager::PowerUpManager(const std::string& spritefile) : _spritefile(spritefile)
{
	_powerup_bar_sprite = LoadTexture(_spritefile.c_str());
}

PowerUpManager::~PowerUpManager()
{
	UnloadTexture(_powerup_bar_sprite);
}

void PowerUpManager::update(const Player& player)
{
	if (player.get_current_power() <= 0)
	{
		return;
	}

	switch (player.get_current_power())
	{
		case 1:
		{
			_color = Color{ 0,219,255,255 };
			break;
		}
		case 2:
		{
			_color = Color{ 255,109,0,255 };
			break;
		}
		case 3:
		{
			_color = Color{ 255,219,85,255 };
			break;
		}
		case 4:
		{
			_color = Color{ 182,109,255,255 };
		}
	}
}

void PowerUpManager::draw(raylib::DrawSession& ds, const Player& player) const
{
	if (player.get_current_power() <= 0)
	{
		return;
	}	// We need to keep this cast for type safety - Raylib uses int for texture dimensions
	// but Rectangle and Vector2 expect floats
	// Cache the value to avoid repeated conversions
	const float spriteWidth = static_cast<float>(_powerup_bar_sprite.width);
	Vector2 dest{ F::SCREEN_WIDTH - spriteWidth - 0.25F * F::BASE_SIZE, 0.25F * F::BASE_SIZE };
	Rectangle source{ 0.0f, 0.0f, spriteWidth, F::BASE_SIZE };
	ds.DrawTexture(_powerup_bar_sprite, source, dest, WHITE);

	dest = Vector2(F::SCREEN_WIDTH - spriteWidth - 0.125F * F::BASE_SIZE, 0.25F * F::BASE_SIZE);
	
	const float barWidth = ceil(player.get_power_timer() * (spriteWidth - 0.25F * F::BASE_SIZE) / POWERUP_DURATION);
	source = Rectangle(0.125F * F::BASE_SIZE, F::BASE_SIZE, barWidth, F::BASE_SIZE);
	ds.DrawTexture(_powerup_bar_sprite, source, dest, _color);
}
