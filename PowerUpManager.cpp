// Corresponding header
#include "PowerUpManager.hpp"

// Third-party headers
#include <raylib.h>

#include <cmath>

// Project headers
#include "Global.hpp"
#include "RLDrawSession.hpp"

PowerUpManager::PowerUpManager(const std::string &spritefile) : _powerup_bar_sprite(spritefile), _spritefile(spritefile)
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
        _color = GlobalColors::POWERUP_1;
        break;
    }
    case 2:
    {
        _color = GlobalColors::POWERUP_2;
        break;
    }
    case 3:
    {
        _color = GlobalColors::POWERUP_3;
        break;
    }
    case 4:
    {
        _color = GlobalColors::POWERUP_4;
    }
    }
}

auto PowerUpManager::get_fill_fraction(const Player &player) const noexcept -> float
{
    if (player.get_current_power() <= 0)
    {
        return 0.0F;
    }
    const float spriteWidth = _powerup_bar_sprite.widthF();
    const float maxFillLogical = spriteWidth - (GlobalConstant::EIGHTH * GlobalConstant::BASE_SIZE) - (GlobalConstant::QUARTER * GlobalConstant::BASE_SIZE);
    const float currentFillLogical = std::ceil(player.get_power_timer() * maxFillLogical / static_cast<float>(GlobalConstant::Int::POWERUP_DURATION));
    return (maxFillLogical > 0.0F) ? (currentFillLogical / maxFillLogical) : 0.0F;
}

void PowerUpManager::draw(raylib::DrawSession &ds, const Player &player, float bannerHeightLogical) const
{
    if (player.get_current_power() <= 0)
    {
        return;
    }
    const float scale = 1.0F; // drawing into banner render texture (logical)
    const float bannerHeightPixels = bannerHeightLogical * scale;
    const float padding = GlobalConstant::BANNER_PADDING;

    const float texW = _powerup_bar_sprite.widthF();
    const float rowH = GlobalConstant::BASE_SIZE;
    const float logicalH = GlobalConstant::BANNER_HEIGHT * GlobalConstant::HALF; // half banner height
    const float scaleH = logicalH / rowH;
    const float drawH = logicalH * scale;
    const float frameWLogical = texW;
    const float fillLeftLogical = GlobalConstant::EIGHTH * GlobalConstant::BASE_SIZE;
    const float fillRightPadding = GlobalConstant::QUARTER * GlobalConstant::BASE_SIZE;

    const float maxFillLogical = frameWLogical - fillLeftLogical - fillRightPadding;
    const float fillFrac = get_fill_fraction(player);
    const float fillWLogical = std::max(0.0F, std::min(maxFillLogical, maxFillLogical * fillFrac));

    const float destFrameW = frameWLogical * scaleH * scale;
    const float windowWidth = GlobalConstant::SCREEN_WIDTH * scale;
    const float destX = windowWidth - padding - destFrameW;
    const float destY = (bannerHeightPixels - drawH) * 0.5F;

    const Rectangle srcFrame{0.0F, 0.0F, frameWLogical, rowH};
    const Rectangle dstFrame{destX, destY, destFrameW, drawH};
    ds.DrawTexturePro(_powerup_bar_sprite.get(), srcFrame, dstFrame, Vector2{0.0F, 0.0F}, 1.0F, GlobalColors::COL_WHITE);

    const Rectangle srcFill{fillLeftLogical, rowH, fillWLogical, rowH};
    const float destFillX = destX + (fillLeftLogical * scaleH * scale);
    const float destFillW = fillWLogical * scaleH * scale;
    const Rectangle dstFill{destFillX, destY, destFillW, drawH};
    ds.DrawTexturePro(_powerup_bar_sprite.get(), srcFill, dstFill, Vector2{0.0F, 0.0F}, 1.0F, _color);
}
