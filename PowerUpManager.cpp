// Corresponding header
#include "PowerUpManager.hpp"

// Third-party headers
#include <raylib.h>

// Project headers
#include "Global.hpp"
#include "RLDrawSession.h"

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

auto PowerUpManager::get_fill_fraction(const Player& player) const noexcept -> float
{
	if (player.get_current_power() <= 0)
	{
		return 0.0f;
	}
	const float spriteWidth = static_cast<float>(_powerup_bar_sprite.width);
	const float maxFillLogical = spriteWidth - 0.125f * GlobalConstant::BASE_SIZE - 0.25f * GlobalConstant::BASE_SIZE;
	const float currentFillLogical = ceil(player.get_power_timer() * maxFillLogical / static_cast<float>(GlobalConstant::Int::POWERUP_DURATION));
	return (maxFillLogical > 0.0f) ? (currentFillLogical / maxFillLogical) : 0.0f;
}

void PowerUpManager::draw(raylib::DrawSession& ds, const Player& player) const
{
	if (player.get_current_power() <= 0)
	{
		return;
	}
	// Rendering of the power bar in the banner is performed by Backbuffer::flip().
	// Main will forward sprite, color and fill fraction to Backbuffer each frame via SetPowerBar().
	(void)ds;
	(void)player;
}






