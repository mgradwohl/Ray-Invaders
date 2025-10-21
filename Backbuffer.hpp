#pragma once
#include <raylib.h>
#include "Global.hpp"
class PowerUpManager;
class BannerUI;
class Backbuffer
{
public:
	Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept;
	~Backbuffer();
	void flip() const noexcept;
	// Access render targets for gameplay and banner drawing from Main
	[[nodiscard]] auto GetGameplayRenderTexture() noexcept -> RenderTexture2D & { return _rtGameplay; }
	[[nodiscard]] auto GetBannerRenderTexture() noexcept -> RenderTexture2D & { return _rtBanner; }

private:
	RenderTexture2D _rtGameplay{}; // SCREEN_WIDTH x SCREEN_HEIGHT
	RenderTexture2D _rtBanner{};   // SCREEN_WIDTH x BANNER_HEIGHT
    float           _scale{1.0F};
};

