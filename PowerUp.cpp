#include <raylib.h>
#include"RLDrawSession.h"
#include "Global.hpp"
#include "PowerUp.hpp"

PowerUp::PowerUp(const std::string& spritefile)
{
	_spritefile = spritefile;
	_powerup_bar_sprite = LoadTexture(_spritefile.c_str());
}

PowerUp::~PowerUp()
{
	UnloadTexture(_powerup_bar_sprite);
}

void PowerUp::update(const Player& player)
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

void PowerUp::draw(raylib::DrawSession& ds, const Player& player) const
{
	if (player.get_current_power() <= 0)
	{
		return;
	}

	Vector2 dest{ SCREEN_WIDTH - _powerup_bar_sprite.width - 0.25F * BASE_SIZE, 0.25F * BASE_SIZE };
	Rectangle source{ 0, 0, _powerup_bar_sprite.width, BASE_SIZE };
	ds.DrawTexture(_powerup_bar_sprite, source, dest, WHITE);

	dest = Vector2(SCREEN_WIDTH - _powerup_bar_sprite.width - 0.125F * BASE_SIZE, 0.25F * BASE_SIZE);
	source = Rectangle(0.125F * BASE_SIZE, BASE_SIZE, ceil(player.get_power_timer() * static_cast<float>(_powerup_bar_sprite.width - 0.25F * BASE_SIZE) / POWERUP_DURATION), BASE_SIZE);
	ds.DrawTexture(_powerup_bar_sprite, source, dest, _color);
}

