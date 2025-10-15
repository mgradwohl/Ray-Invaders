#pragma once
#include <raylib.h>
#include "Global.hpp"
class Backbuffer
{
public:
	Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept;
	~Backbuffer();
	void flip() const noexcept;
    [[nodiscard]] auto GetRenderTexture() noexcept -> RenderTexture2D & { return _backbuffer; }

    [[nodiscard]] auto GetTexture() const noexcept -> const Texture2D
    {
		return _backbuffer.texture;
	}

private:
	RenderTexture2D _backbuffer{};
    float           _scale{1.0F};
};

