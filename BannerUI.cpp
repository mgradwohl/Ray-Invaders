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
    //  const float scale = 1.0f; // Banner RT uses logical pixels (no scaling here)
    const float bannerHeightLogical = GlobalConstant::BANNER_HEIGHT;

    // Draw centered banner image if available (scaled to banner height)
    if (_banner.id() > 0)
    {
        const Texture2D &bannerTex = _banner.get();
        const float bsrcW = static_cast<float>(bannerTex.width);
        const float bsrcH = static_cast<float>(bannerTex.height);
        const Rectangle bsrc{0.0f, 0.0f, bsrcW, bsrcH};
        const float s = (bsrcH > 0.0f) ? (bannerHeightLogical / bsrcH) : 1.0f;
        // Pixel-perfect integral destination
        const float bdW = std::round(bsrcW * s);
        const float bdH = std::round(bannerHeightLogical);
        const float windowWidth = GlobalConstant::SCREEN_WIDTH;
        const float bdX = std::floor((windowWidth - bdW) * 0.5f);
        const float bdY = 0.0f;
        const Rectangle bdst{bdX, bdY, bdW, bdH};
        ds.DrawTexturePro(_banner.get(), bsrc, bdst, Vector2{0.0f, 0.0f}, 1.0f, WHITE);
    }

    // Level text (left)
    if (_level >= 0)
    {
        // Font sized to fit nicely in 20px banner
        const int fontSize = 10;
        const std::string text = "Level: " + std::to_string(_level);
        const float drawX = GlobalConstant::BANNER_PADDING;
        const float drawY = (bannerHeightLogical - static_cast<float>(fontSize)) * 0.5f;
        ds.DrawText(text.c_str(), static_cast<int>(drawX), static_cast<int>(drawY), fontSize,
                    WHITE);
    }

    // Power bar (right) via manager
    if (_pum)
    {
        _pum->draw(ds, player, bannerHeightLogical);
    }
}
