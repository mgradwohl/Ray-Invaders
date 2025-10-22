// Corresponding header
#include "PowerUpManager.hpp"

// Third-party headers
#include <raylib.h>

// Project headers
#include "Global.hpp"
#include "RLDrawSession.h"

PowerUpManager::PowerUpManager(const std::string &spritefile)
    : _powerup_bar_sprite(spritefile), _spritefile(spritefile)
{
}

PowerUpManager::~PowerUpManager()
{
    // RAII wrapper handles texture cleanup automatically
}

void PowerUpManager::update(const Player &player)
{
    if (player.get_current_power() <= 0)
    {
        return;
    }

    switch (player.get_current_power())
    {
    case 1:
    {
        _color = Color{0, 219, 255, 255};
        break;
    }
    case 2:
    {
        _color = Color{255, 109, 0, 255};
        break;
    }
    case 3:
    {
        _color = Color{255, 219, 85, 255};
        break;
    }
    case 4:
    {
        _color = Color{182, 109, 255, 255};
    }
    }
}

auto PowerUpManager::get_fill_fraction(const Player &player) const noexcept -> float
{
    if (player.get_current_power() <= 0)
    {
        return 0.0f;
    }
    const float spriteWidth = _powerup_bar_sprite.widthF();
    const float maxFillLogical =
        spriteWidth - 0.125f * GlobalConstant::BASE_SIZE - 0.25f * GlobalConstant::BASE_SIZE;
    const float currentFillLogical =
        ceil(player.get_power_timer() * maxFillLogical /
             static_cast<float>(GlobalConstant::Int::POWERUP_DURATION));
    return (maxFillLogical > 0.0f) ? (currentFillLogical / maxFillLogical) : 0.0f;
}

void PowerUpManager::draw(raylib::DrawSession &ds, const Player &player,
                          float bannerHeightLogical) const
{
    if (player.get_current_power() <= 0)
    {
        return;
    }
    const float scale = 1.0f; // drawing into banner render texture (logical)
    const float bannerHeightPixels = bannerHeightLogical * scale;
    const float padding = GlobalConstant::BANNER_PADDING;

    const float texW = _powerup_bar_sprite.widthF();
    const float rowH = GlobalConstant::BASE_SIZE;
    const float logicalH = GlobalConstant::BANNER_HEIGHT * 0.5f; // half banner height
    const float scaleH = logicalH / rowH;
    const float drawH = logicalH * scale;
    const float frameWLogical = texW;
    const float fillLeftLogical = 0.125f * GlobalConstant::BASE_SIZE;
    const float fillRightPadding = 0.25f * GlobalConstant::BASE_SIZE;

    const float maxFillLogical = frameWLogical - fillLeftLogical - fillRightPadding;
    const float fillFrac = get_fill_fraction(player);
    const float fillWLogical = std::max(0.0f, std::min(maxFillLogical, maxFillLogical * fillFrac));

    const float destFrameW = frameWLogical * scaleH * scale;
    const float windowWidth = GlobalConstant::SCREEN_WIDTH * scale;
    const float destX = windowWidth - padding - destFrameW;
    const float destY = (bannerHeightPixels - drawH) * 0.5f;

    const Rectangle srcFrame{0.0f, 0.0f, frameWLogical, rowH};
    const Rectangle dstFrame{destX, destY, destFrameW, drawH};
    ds.DrawTexturePro(_powerup_bar_sprite.get(), srcFrame, dstFrame, Vector2{0.0f, 0.0f}, 1.0f,
                      WHITE);

    const Rectangle srcFill{fillLeftLogical, rowH, fillWLogical, rowH};
    const float destFillX = destX + (fillLeftLogical * scaleH * scale);
    const float destFillW = fillWLogical * scaleH * scale;
    const Rectangle dstFill{destFillX, destY, destFillW, drawH};
    ds.DrawTexturePro(_powerup_bar_sprite.get(), srcFill, dstFill, Vector2{0.0f, 0.0f}, 1.0f,
                      _color);
}
