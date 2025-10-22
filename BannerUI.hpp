#pragma once
#include <string>

#include <raylib.h>

#include "Global.hpp"
#include "RLDrawSession.h"
#include "RLTexture2D.hpp"

class PowerUpManager;
class Player;

class BannerUI {
public:
    explicit BannerUI(const PowerUpManager* pum) noexcept;
    ~BannerUI() = default;

    void setLevel(int level) noexcept { _level = level; }
    [[nodiscard]] GameTypes::Coordinate getHeight() const noexcept { return static_cast<GameTypes::Coordinate>(_banner.height()); }

    void draw(raylib::DrawSession& ds, const Player& player) const noexcept;

private:
    raylib::Texture2DFile _banner; // owned RAII
    const PowerUpManager* _pum{nullptr};
    int _level{-1};
};
