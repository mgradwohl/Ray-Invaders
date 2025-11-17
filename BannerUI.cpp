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
    // Reuse the player sprite as a small life icon (no extra asset required)
    _life_icon.load("Resources/Images/Player.png");
    _life_icon.setPointFiltering();
    _font = LoadFontEx("Resources/Fonts/abduction2002.ttf", 8, nullptr, 0);
}

void BannerUI::draw(raylib::DrawSession &ds, const Player &player) const noexcept
{
    //  const float scale = 1.0F; // Banner RT uses logical pixels (no scaling here)
    const float bannerHeightLogical = GlobalConstant::BANNER_HEIGHT;

    // Draw centered banner image if available (scaled to banner height)
    if (_banner.id() > 0)
    {
        const Texture2D &bannerTex = _banner.get();
        const auto bsrcW = static_cast<float>(bannerTex.width);
        const auto bsrcH = static_cast<float>(bannerTex.height);
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
        ds.DrawText(text, static_cast<int>(drawX), static_cast<int>(drawY), fontSize, GlobalColors::COL_WHITE);

        // ds.DrawTextEx(_font, text, static_cast<int>(drawX), static_cast<int>(drawY), 8, 2, GlobalColors::COL_WHITE);
    }

    // Lives icons (draw to the right of the level text). We'll draw up to a reasonable
    // number of icons (e.g., clamp at 8) to avoid overflow, scaling the icon to fit the
    // banner height.
    const int lives = player.get_lives();
    if (lives > 0)
    {
        const int maxIcons = 8;
        const int iconsToDraw = std::min(lives, maxIcons) - 1;

        // Position icons to the right of the level text (if present) or at padding
        float iconsStartX = GlobalConstant::BANNER_PADDING;
        if (_level >= 0)
        {
            const int fontSize = 10; // same as above
            const std::string levelText = "Level: " + std::to_string(_level);
            const int tW = MeasureText(levelText.c_str(), fontSize);
            iconsStartX += static_cast<float>(tW) + GlobalConstant::BANNER_PADDING;
        }

        // Draw icons scaled to about 75% of banner height
        const Texture2D &iconTex = _life_icon.get();
        if (iconTex.id > 0)
        {
            // Player.png contains multiple frames; use the first frame (BASE_SIZE x BASE_SIZE)
            const float srcW = GlobalConstant::BASE_SIZE;
            const float srcH = GlobalConstant::BASE_SIZE;

            const float iconDstH = bannerHeightLogical * GlobalConstant::HALF;
            const float iconDstW = iconDstH; // square frame

            float x = iconsStartX;
            const float y = (bannerHeightLogical - iconDstH) * GlobalConstant::HALF;

            for (int i = 0; i < iconsToDraw; ++i)
            {
                const Rectangle src{0.0F, 0.0F, srcW, srcH};
                const Rectangle dst{x, y, iconDstW, iconDstH};
                ds.DrawTexturePro(_life_icon.get(), src, dst, Vector2{0.0F, 0.0F}, 1.0F, GlobalColors::COL_WHITE);
                x += iconDstW + 2.0F; // small spacing between icons
            }
        }
    }

    // Power bar (right) via manager
    if (_pum)
    {
        _pum->draw(ds, player, bannerHeightLogical);
    }
}
