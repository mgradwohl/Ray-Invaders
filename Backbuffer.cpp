// Corresponding header
#include "Backbuffer.hpp"

// Standard library
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "RLDrawSession.hpp"
// Banner height is fixed via GlobalConstant::Int::BANNER_HEIGHT

Backbuffer::Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept
{
    _rtGameplay.create(width, height);
    _rtBanner.create(width, static_cast<GameTypes::Coordinate>(GlobalConstant::Int::BANNER_HEIGHT));
    _scale = scale;
}

Backbuffer::~Backbuffer() = default;

void Backbuffer::flip() const noexcept
{
    // Compose final frame: draw banner at top, gameplay below
    const Vector2 pos{0.0f, 0.0f};

    // Use dimensions directly when constructing rectangles
    const Rectangle gameplaySrc{0.0f, 0.0f, _rtGameplay.widthF(), -_rtGameplay.heightF()};
    const Rectangle bannerSrc{0.0f, 0.0f, _rtBanner.widthF(), -_rtBanner.heightF()};

    raylib::DrawSession screenDs{BLACK};

    // 1) Draw banner texture at top
    const Rectangle bannerDst{0.0f, 0.0f, _rtBanner.widthF() * _scale, _rtBanner.heightF() * _scale};
    screenDs.DrawTexturePro(_rtBanner.texture(), bannerSrc, bannerDst, pos, 1.0F, WHITE);

    // 2) Draw gameplay below banner strip
    const Rectangle gameplayDst{0.0f, GlobalConstant::BANNER_HEIGHT * _scale, _rtGameplay.widthF() * _scale, _rtGameplay.heightF() * _scale};
    screenDs.DrawTexturePro(_rtGameplay.texture(), gameplaySrc, gameplayDst, pos, 1.0F, WHITE);
}
