#pragma once
#include <raylib.h>
class Backbuffer
{
public:
	Backbuffer(unsigned short width, unsigned short height, unsigned short scale);
	~Backbuffer();
	const void flip() const;

	RenderTexture2D& GetRenderTexture()
	{
		return _backbuffer;
	}

	const Texture2D GetTexture() const
	{
		return _backbuffer.texture;
	}

private:
	RenderTexture2D _backbuffer;
	float _scale;
};

