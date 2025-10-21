#pragma once
#include <raylib.h>
#include <string>
#include "Global.hpp"
#include "RLDrawSession.h"

class PowerUpManager;
class Player;

class BannerUI {
public:
    explicit BannerUI(const PowerUpManager* pum) noexcept;
    ~BannerUI();

    void setLevel(int level) noexcept { _level = level; }
    [[nodiscard]] GameTypes::Coordinate getHeight() const noexcept { return static_cast<GameTypes::Coordinate>(_banner.height); }

    void draw(raylib::DrawSession& ds, const Player& player) const noexcept;

private:
    Texture2D _banner{}; // owned
    const PowerUpManager* _pum{nullptr};
    int _level{-1};
};
