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
	
	// We can't avoid these casts since Raylib's texture dimensions are integers
	// but Rectangle expects floats. The casts are necessary for type safety.
	const float textureWidth = static_cast<float>(_backbuffer.texture.width);
	const float textureHeight = static_cast<float>(_backbuffer.texture.height);
	
	const Rectangle source{ 0.0f, 0.0f, textureWidth, -textureHeight };

	BeginDrawing();
	const Rectangle dest{ 0.0f, 0.0f, textureWidth * _scale, textureHeight * _scale };
	DrawTexturePro(_backbuffer.texture, source, dest, pos, 0.0F, WHITE);
	EndDrawing();
}
