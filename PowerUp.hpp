#pragma once
#include "RLDrawSession.h"
#include "Player.hpp"
class PowerUp
{
public:
	PowerUp(const std::string& spritefile);
	~PowerUp();
	void update(Player& player);
	void draw(raylib::DrawSession& ds, Player& player);

private:
	Texture2D powerup_bar_sprite;
	Color color = WHITE;
	std::string _spritefile;
};

