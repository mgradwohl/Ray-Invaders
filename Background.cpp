#include "Background.hpp"

Background::Background(std::string spritefile)
{
	backgroundsprite = LoadTexture(spritefile.c_str());

	source.x = (backgroundsprite.width - SCREEN_WIDTH) / 2;
	source.y = backgroundsprite.height - SCREEN_HEIGHT;
	source.width = SCREEN_WIDTH;
	source.height = SCREEN_HEIGHT;
}

Background::~Background()
{
	UnloadTexture(backgroundsprite);
}

void Background::draw(raylib::DrawSession& ds)
{
	Vector2 pos{ 0,0 };
	ds.DrawTexture(backgroundsprite, source, pos, WHITE);

	//ds.DrawTexture(backgroundsprite, x, y, WHITE);
}

void Background::update(Player& player)
{
	if (IsKeyDown(KEY_RIGHT))
	{
		//player moving right, move background from right to left
		source.x++;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		//player moving left, move background from left to right
		source.x--;
	}

	if (source.x >= backgroundsprite.width)
	{
		source.x = backgroundsprite.width;
	}

	if (source.x <= 0)
	{
		source.x = 0;
	}

	source.y--;
	if (source.y <= 0)
	{
		source.y = 0;
	}
}
