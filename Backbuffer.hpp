#pragma once
#include <raylib.h>
#include "Global.hpp"
class Backbuffer
{
public:
	Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept;
	~Backbuffer();
	void flip() const noexcept;
    void SetBanner(const Texture2D* banner) noexcept { _banner = banner; }
	void SetLevel(int level) noexcept { _level = level; }
	// Queue power-up bar UI to be drawn in the banner strip (fraction in [0,1])
	void SetPowerBar(const Texture2D* tex, Color color, float fraction) noexcept {
		_powerBarTex = tex; _powerBarColor = color; _powerBarFraction = fraction;
	}
    [[nodiscard]] auto GetRenderTexture() noexcept -> RenderTexture2D & { return _backbuffer; }

    [[nodiscard]] auto GetTexture() const noexcept -> const Texture2D
    {
		return _backbuffer.texture;
	}

private:
	RenderTexture2D _backbuffer{};
    float           _scale{1.0F};
	const Texture2D* _banner{nullptr}; // non-owning pointer managed by caller
	int              _level{-1};
	// Power-up UI queued data
	const Texture2D* _powerBarTex{nullptr};
	Color            _powerBarColor{WHITE};
	float            _powerBarFraction{0.0f};
};

