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
	const Vector2 pos{ 0.0f, 0.0f };
	const Rectangle source = { 0.0f, 0.0f, static_cast<float>(_backbuffer.texture.width), -static_cast<float>(_backbuffer.texture.height) };

	BeginDrawing();
	const Rectangle dest = { 0.0f, 0.0f, static_cast<float>(_backbuffer.texture.width) * _scale, static_cast<float>(_backbuffer.texture.height) * _scale };
	DrawTexturePro(_backbuffer.texture, source, dest, pos, 0.0F, WHITE);
	EndDrawing();
}
