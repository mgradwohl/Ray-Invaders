#pragma once

#include <string>

#include <raylib.h>

#include "Player.hpp"
#include "RLDrawSession.hpp"
#include "RLTexture2D.hpp"

class Background
{
  public:
    Background(const std::string &spritefile);

    Background() = default;
    Background(const Background &) = delete;
    Background(Background &&) = delete;
    auto operator=(const Background &) -> Background & = delete;
    auto operator=(Background &&) -> Background & = delete;

    ~Background() = default;

    void draw(raylib::DrawSession &ds) const;
    void update(Player &player) noexcept;
    void reset() noexcept;

  private:
    raylib::Texture2DFile _backgroundsprite;
    Rectangle _source{0.0F, 0.0F, 0.0F, 0.0F};
};
