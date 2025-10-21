#pragma once

#include <string>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLTexture2D.hpp"
#include "Player.hpp"

class Background
{
public:

	Background() = default;
	Background(const std::string& spritefile);
	~Background() = default;
	void draw(raylib::DrawSession& ds) const;
	void update(Player& player) noexcept;
	void reset() noexcept;

private:
	raylib::Texture2DFile _backgroundsprite;
    Rectangle _source{0.0F, 0.0F, 0.0F, 0.0F};
};

