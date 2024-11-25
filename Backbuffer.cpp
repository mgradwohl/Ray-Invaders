#include <raylib.h>
#include "Backbuffer.hpp"

Backbuffer::Backbuffer(unsigned short width, unsigned short height, unsigned short scale)
{
	_backbuffer = ::LoadRenderTexture(width, height);
	_scale = scale;
}

Backbuffer::~Backbuffer()
{
	UnloadRenderTexture(_backbuffer);
}

const void Backbuffer::flip() const
{
	Vector2 pos{ 0,0 };
	Rectangle source = { 0, 0, _backbuffer.texture.width, -_backbuffer.texture.height };

	BeginDrawing();
	Rectangle dest = { 0, 0, _backbuffer.texture.width * _scale, _backbuffer.texture.height * _scale};
	DrawTexturePro(_backbuffer.texture, source, dest, pos, 0.0f, WHITE);
	EndDrawing();

}
