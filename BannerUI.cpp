#include "BannerUI.hpp"

// Standard library headers
#include <cmath>
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Player.hpp"
#include "PowerUpManager.hpp"

BannerUI::BannerUI(const PowerUpManager *pum) noexcept : _pum(pum)
{
    _banner.load("Resources/Images/RayInvadersScaled.png");
    _banner.setPointFiltering();
}

void BannerUI::draw(raylib::DrawSession &ds, const Player &player) const noexcept
{
    //  const float scale = 1.0F; // Banner RT uses logical pixels (no scaling here)
    const float bannerHeightLogical = GlobalConstant::BANNER_HEIGHT;

    // Draw centered banner image if available (scaled to banner height)
    if (_banner.id() > 0)
    {
        const Texture2D &bannerTex = _banner.get();
        const float bsrcW = static_cast<float>(bannerTex.width);
        const float bsrcH = static_cast<float>(bannerTex.height);
        const Rectangle bsrc{0.0F, 0.0F, bsrcW, bsrcH};
        const float s = (bsrcH > 0.0F) ? (bannerHeightLogical / bsrcH) : 1.0F;
        // Pixel-perfect integral destination
        const float bdW = std::round(bsrcW * s);
        const float bdH = std::round(bannerHeightLogical);
        const float windowWidth = GlobalConstant::SCREEN_WIDTH;
        const float bdX = std::floor((windowWidth - bdW) * 0.5F);
        const float bdY = 0.0F;
        const Rectangle bdst{bdX, bdY, bdW, bdH};
    ds.DrawTexturePro(_banner.get(), bsrc, bdst, Vector2{0.0F, 0.0F}, 1.0F, GlobalColors::COL_WHITE);
    }

    // Level text (left)
    if (_level >= 0)
    {
        // Font sized to fit nicely in 20px banner
        const int fontSize = 10;
        const std::string text = "Level: " + std::to_string(_level);
        const float drawX = GlobalConstant::BANNER_PADDING;
        const float drawY = (bannerHeightLogical - static_cast<float>(fontSize)) * 0.5F;
    ds.DrawText(text.c_str(), static_cast<int>(drawX), static_cast<int>(drawY), fontSize, GlobalColors::COL_WHITE);
    }

    // Power bar (right) via manager
    if (_pum)
    {
        _pum->draw(ds, player, bannerHeightLogical);
    }
}
