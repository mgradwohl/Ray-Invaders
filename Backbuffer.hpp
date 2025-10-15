#pragma once
#include <raylib.h>
class Backbuffer
{
public:
	Backbuffer(unsigned short width, unsigned short height, unsigned short scale) noexcept;
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

