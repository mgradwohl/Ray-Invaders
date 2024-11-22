#pragma once
class Backbuffer
{
public:
	Backbuffer(unsigned short width, unsigned short height, unsigned short scale);
	~Backbuffer();
	void flip();

	RenderTexture2D& GetRenderTexture()
	{
		return _backbuffer;
	}

	Texture2D GetTexture()
	{
		return _backbuffer.texture;
	}

private:
	RenderTexture2D _backbuffer;
	unsigned short _scale;
};

