// Corresponding header
#include "Background.hpp"

// Standard library headers
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Player.hpp"
#include "RLDrawSession.hpp"
#include "RLTexture2D.hpp"

Background::Background(const std::string &spritefile) : _backgroundsprite(spritefile) { reset(); }

void Background::draw(raylib::DrawSession &ds) const
{
    const Vector2 pos{0, 0};
    ds.DrawTexture(_backgroundsprite.get(), _source, pos, WHITE);
}

void Background::update([[maybe_unused]] Player &player) noexcept
{
    (void) player; // unused
    if (IsKeyDown(KEY_RIGHT))
    {
        // player moving right, move background from right to left
        _source.x += 1.0F;
    }

    if (IsKeyDown(KEY_LEFT))
    {
        // player moving left, move background from left to right
        _source.x -= 1.0F;
    }

    // Use a named constant for clarity and to avoid repeated casts
    const float bgWidth = _backgroundsprite.widthF();
    if (_source.x >= bgWidth)
    {
        _source.x = bgWidth;
    }

    if (_source.x <= 0.0F)
    {
        _source.x = 0.0F;
    }

    // Decrease Y position by 1.0F rather than integer decrement
    _source.y -= 1.0F;
    if (_source.y <= 0.0F)
    {
        _source.y = 0.0F;
    }
}

void Background::reset() noexcept
{
    // Calculate center position using float arithmetic
    // Texture dimensions as floats - no casts needed with widthF/heightF methods
    // Cache these values to avoid repeated method calls throughout the code
    const float bgWidth = _backgroundsprite.widthF();
    const float bgHeight = _backgroundsprite.heightF();

    constexpr float CENTER_OFFSET = 0.5F;
    _source.x = (bgWidth - GlobalConstant::SCREEN_WIDTH) * CENTER_OFFSET;
    _source.y = bgHeight - GlobalConstant::SCREEN_HEIGHT;
    _source.width = GlobalConstant::SCREEN_WIDTH;
    _source.height = GlobalConstant::SCREEN_HEIGHT;
}
