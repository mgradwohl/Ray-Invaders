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
	Texture2D _powerup_bar_sprite;
	Color _color = WHITE;
	std::string _spritefile;
};

