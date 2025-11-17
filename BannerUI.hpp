#pragma once

#include <raylib.h>

#include "Global.hpp"
#include "RLDrawSession.hpp"
#include "RLTexture2D.hpp"

class PowerUpManager;
class Player;

class BannerUI
{
  public:
    BannerUI(const BannerUI &) = delete;
    BannerUI(BannerUI &&) = delete;
    auto operator=(const BannerUI &) -> BannerUI & = delete;
    auto operator=(BannerUI &&) -> BannerUI & = delete;
    explicit BannerUI(const PowerUpManager *pum) noexcept;
    ~BannerUI() = default;

    void setLevel(int level) noexcept
    {
        _level = level;
    }
    [[nodiscard]] auto getHeight() const noexcept -> GameTypes::Coordinate
    {
        return static_cast<GameTypes::Coordinate>(_banner.height());
    }

    void draw(raylib::DrawSession &ds, const Player &player) const noexcept;

  private:
    raylib::Texture2DFile _banner; // owned RAII
    // Small icon used to represent remaining lives in the banner
    raylib::Texture2DFile _life_icon;
    const PowerUpManager *_pum{nullptr};
    int _level{-1};
    Font _font{};
};
