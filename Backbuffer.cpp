// Corresponding header
#include "Backbuffer.hpp"
// Project headers
#include "RLDrawSession.h"

// Third-party headers
#include <raylib.h>
// Standard library
#include <string>
// Banner height is fixed via GlobalConstant::Int::BANNER_HEIGHT

Backbuffer::Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept
{
	_rtGameplay = ::LoadRenderTexture(width, height);
	_rtBanner   = ::LoadRenderTexture(width, static_cast<GameTypes::Coordinate>(GlobalConstant::Int::BANNER_HEIGHT));
	_scale = scale;
}

Backbuffer::~Backbuffer()
{
	UnloadRenderTexture(_rtGameplay);
	UnloadRenderTexture(_rtBanner);
}

void Backbuffer::flip() const noexcept
{ 
	// Compose final frame: draw banner at top, gameplay below
	const float scale = _scale;
	const float bannerHeightLogical = GlobalConstant::BANNER_HEIGHT;
	const float bannerHeightPixels = bannerHeightLogical * scale;

	const Vector2 pos{ 0.0f, 0.0f };
    
	// Texture dimensions as floats for Rectangle construction
	const float gameplayW = static_cast<float>(_rtGameplay.texture.width);
	const float gameplayH = static_cast<float>(_rtGameplay.texture.height);
	const float bannerW   = static_cast<float>(_rtBanner.texture.width);
	const float bannerH   = static_cast<float>(_rtBanner.texture.height);
	const Rectangle gameplaySrc{ 0.0f, 0.0f, gameplayW, -gameplayH };
	const Rectangle bannerSrc{ 0.0f, 0.0f, bannerW, -bannerH };

	raylib::DrawSession screenDs{BLACK};

	// 1) Draw banner texture at top
	const Rectangle bannerDst{ 0.0f, 0.0f, bannerW * scale, bannerH * scale };
	screenDs.DrawTexturePro(_rtBanner.texture, bannerSrc, bannerDst, pos, 1.0F, WHITE);

	// 2) Draw gameplay below banner strip
	const Rectangle gameplayDst{ 0.0f, bannerHeightPixels, gameplayW * scale, gameplayH * scale };
	screenDs.DrawTexturePro(_rtGameplay.texture, gameplaySrc, gameplayDst, pos, 1.0F, WHITE);
}
