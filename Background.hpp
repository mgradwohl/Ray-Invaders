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
	void update(Player& player);
	void reset();

private:
	Texture2D _backgroundsprite;
	Rectangle _source{ 0, 0, 0, 0 };
};

