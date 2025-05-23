#pragma once

#include <string>
#include <raylib.h>
#include "RLDrawSession.h"
#include "Player.hpp"

class Background
{
public:

	Background() = default;
	Background(const std::string& spritefile);
	~Background();
	void draw(raylib::DrawSession& ds) const;
	void update(Player& player) noexcept;
	void reset() noexcept;

private:
	Texture2D _backgroundsprite{}; // Already properly initialized
	Rectangle _source{ 0.0f, 0.0f, 0.0f, 0.0f };
};

