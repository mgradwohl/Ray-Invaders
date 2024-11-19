#pragma once

#include <string>
#include <raylib.h>
#include "RLDrawSession.h"
#include "Player.hpp"

class Background
{
public:

	Background() = default;
	Background(std::string spritefile);
	~Background();
	void draw(raylib::DrawSession& ds);
	void update(Player& player);

private:
	Texture2D backgroundsprite;
	Rectangle source{ 0, 0, 0, 0 };
};

