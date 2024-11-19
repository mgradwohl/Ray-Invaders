#include <raylib.h>
#include"RLDrawSession.h"
#include "Global.hpp"
#include "PowerUp.hpp"

PowerUp::PowerUp(std::string spritefile)
{
	_spritefile = spritefile;
	powerup_bar_sprite = LoadTexture(_spritefile.c_str());
}

PowerUp::~PowerUp()
{
	UnloadTexture(powerup_bar_sprite);
}

void PowerUp::update(Player& player)
{
	if (player.get_current_power() <= 0)
		return;

	switch (player.get_current_power())
	{
		case 1:
		{
			color = Color{ 0,219,255,255 };
			break;
		}
		case 2:
		{
			color = Color{ 255,109,0,255 };
			break;
		}
		case 3:
		{
			color = Color{ 255,219,85,255 };
			break;
		}
		case 4:
		{
			color = Color{ 182,109,255,255 };
		}
	}
}

void PowerUp::draw(raylib::DrawSession& ds, Player& player)
{
	if (player.get_current_power() <= 0)
		return;

	Vector2 dest{ SCREEN_WIDTH - powerup_bar_sprite.width - 0.25f * BASE_SIZE, 0.25f * BASE_SIZE };
	Rectangle source{ 0, 0, powerup_bar_sprite.width, BASE_SIZE };
	ds.DrawTexture(powerup_bar_sprite, source, dest, WHITE);

	dest = Vector2(SCREEN_WIDTH - powerup_bar_sprite.width - 0.125f * BASE_SIZE, 0.25f * BASE_SIZE);
	source = Rectangle(0.125f * BASE_SIZE, BASE_SIZE, ceil(player.get_power_timer() * static_cast<float>(powerup_bar_sprite.width - 0.25f * BASE_SIZE) / POWERUP_DURATION), BASE_SIZE);
	ds.DrawTexture(powerup_bar_sprite, source, dest, color);
}

