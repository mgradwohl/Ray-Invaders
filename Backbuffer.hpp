#pragma once
#include <raylib.h>

#include "Global.hpp"
#include "RLRenderTexture2D.hpp"
class PowerUpManager;
class BannerUI;
class Backbuffer
{
  public:
    Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept;
    ~Backbuffer();
    Backbuffer(const Backbuffer &) = delete;
    Backbuffer &operator=(const Backbuffer &) = delete;
    Backbuffer(Backbuffer &&) = delete;
    Backbuffer &operator=(Backbuffer &&) = delete;
    void flip() const noexcept;
    // Access render targets for gameplay and banner drawing from Main
    [[nodiscard]] auto GetGameplayRenderTexture() noexcept -> raylib::RenderTexture2D &
    {
        return _rtGameplay;
    }
    [[nodiscard]] auto GetBannerRenderTexture() noexcept -> raylib::RenderTexture2D &
    {
        return _rtBanner;
    }

  private:
    raylib::RenderTexture2D _rtGameplay; // SCREEN_WIDTH x SCREEN_HEIGHT
    raylib::RenderTexture2D _rtBanner;   // SCREEN_WIDTH x BANNER_HEIGHT
    float _scale{1.0F};
};
