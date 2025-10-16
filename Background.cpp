// Corresponding header
#include "Background.hpp"

// Standard library headers
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Player.hpp"
#include "RLDrawSession.h"

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
}

void Background::update([[maybe_unused]] Player& player) noexcept
{
	(void) player; //unused
	if (IsKeyDown(KEY_RIGHT))
	{
		//player moving right, move background from right to left
		_source.x += 1.0f;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		//player moving left, move background from left to right
		_source.x -= 1.0f;
	}
	
	// Use a named constant for clarity and to avoid repeated casts
	const float bgWidth = static_cast<float>(_backgroundsprite.width);
	if (_source.x >= bgWidth)
	{
		_source.x = bgWidth;
	}

	if (_source.x <= 0.0f)
	{
		_source.x = 0.0f;
	}

	// Decrease Y position by 1.0f rather than integer decrement
	_source.y -= 1.0f;
	if (_source.y <= 0.0f)
	{
		_source.y = 0.0f;
	}
}

void Background::reset() noexcept
{
	// Calculate center position using float arithmetic
	// We need to keep these casts since Raylib texture dimensions are integers
	// but Rectangle fields are floats
	// Cache these values to avoid repeated casts throughout the code
	const float bgWidth = static_cast<float>(_backgroundsprite.width);
	const float bgHeight = static_cast<float>(_backgroundsprite.height);
	
	_source.x = (bgWidth - GlobalConstant::SCREEN_WIDTH) * 0.5f;
	_source.y = bgHeight - GlobalConstant::SCREEN_HEIGHT;
	_source.width = GlobalConstant::SCREEN_WIDTH;
	_source.height = GlobalConstant::SCREEN_HEIGHT;
}






