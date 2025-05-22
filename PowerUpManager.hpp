#pragma once
#include "RLDrawSession.h"
#include "Player.hpp"

class PowerUpManager
{
public:
	PowerUpManager(const std::string& spritefile);
	~PowerUpManager();
	void update(const Player& player);
	void draw(raylib::DrawSession& ds, const Player& player) const;

private:	Texture2D _powerup_bar_sprite;
	Color _color = WHITE;
	const std::string _spritefile;
};
