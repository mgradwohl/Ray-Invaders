#pragma once
#include <raylib.h>
class Backbuffer
{
public:
	Backbuffer(unsigned short width, unsigned short height, unsigned short scale) noexcept;
	~Backbuffer();
	void flip() const noexcept;
	[[nodiscard]] RenderTexture2D& GetRenderTexture() noexcept
	{
		return _backbuffer;
	}

	[[nodiscard]] const Texture2D GetTexture() const noexcept
	{
		return _backbuffer.texture;
	}

private:
	RenderTexture2D _backbuffer;
	float _scale;
};

