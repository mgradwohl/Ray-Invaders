#include <raylib.h>
#include "Backbuffer.hpp"

Backbuffer::Backbuffer(unsigned short width, unsigned short height, unsigned short scale) noexcept
{
	_backbuffer = ::LoadRenderTexture(width, height);
	_scale = scale;
}

Backbuffer::~Backbuffer()
{
	UnloadRenderTexture(_backbuffer);
}

void Backbuffer::flip() const noexcept
{
	const Vector2 pos{ 0,0 };
	const Rectangle source = { 0, 0, _backbuffer.texture.width, -_backbuffer.texture.height };

	BeginDrawing();
	const Rectangle dest = { 0, 0, _backbuffer.texture.width * _scale, _backbuffer.texture.height * _scale};
	DrawTexturePro(_backbuffer.texture, source, dest, pos, 0.0F, WHITE);
	EndDrawing();
}
