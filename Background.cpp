#include "Background.hpp"

Background::Background(const std::string& spritefile)
{
	_backgroundsprite = LoadTexture(spritefile.c_str());
	reset();
}

Background::~Background()
{
	UnloadTexture(_backgroundsprite);
}

void Background::draw(raylib::DrawSession& ds) const
{
	const Vector2 pos{ 0,0 };
	ds.DrawTexture(_backgroundsprite, _source, pos, WHITE);

	//ds.DrawTexture(backgroundsprite, x, y, WHITE);
}

void Background::update(Player& player)
{
	if (IsKeyDown(KEY_RIGHT))
	{
		//player moving right, move background from right to left
		_source.x++;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		//player moving left, move background from left to right
		_source.x--;
	}

	if (_source.x >= _backgroundsprite.width)
	{
		_source.x = _backgroundsprite.width;
	}

	if (_source.x <= 0)
	{
		_source.x = 0;
	}

	_source.y--;
	if (_source.y <= 0)
	{
		_source.y = 0;
	}
}

void Background::reset()
{
	_source.x = (_backgroundsprite.width - SCREEN_WIDTH) / 2;
	_source.y = _backgroundsprite.height - SCREEN_HEIGHT;
	_source.width = SCREEN_WIDTH;
	_source.height = SCREEN_HEIGHT;
}
