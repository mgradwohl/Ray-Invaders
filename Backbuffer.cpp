// Corresponding header
#include "Backbuffer.hpp"

// Third-party headers
#include <raylib.h>
// Standard library
#include <string>

Backbuffer::Backbuffer(GameTypes::Coordinate width, GameTypes::Coordinate height, GameTypes::Size scale) noexcept
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
	// Compose final frame: draw banner at top, gameplay below
	const float scale = _scale;
	const float bannerHeightLogical = GlobalConstant::BANNER_HEIGHT;
	const float bannerHeightPixels = bannerHeightLogical * scale;

	const Vector2 pos{ 0.0f, 0.0f };
    
	// We can't avoid these casts since Raylib's texture dimensions are integers
	// but Rectangle expects floats. The casts are necessary for type safety.
	const float textureWidth = static_cast<float>(_backbuffer.texture.width);
	const float textureHeight = static_cast<float>(_backbuffer.texture.height);
	const Rectangle source{ 0.0f, 0.0f, textureWidth, -textureHeight };

	BeginDrawing();
	ClearBackground(BLACK);
	// 1) Draw banner in top strip if provided
	if (_banner != nullptr && _banner->id > 0)
	{
		const float bsrcW = static_cast<float>(_banner->width);
		const float bsrcH = static_cast<float>(_banner->height);
		const Rectangle bsrc{ 0.0f, 0.0f, bsrcW, bsrcH };
		// Scale to desired logical height (BANNER_HEIGHT) preserving aspect ratio
		const float s = (bsrcH > 0.0f) ? (bannerHeightLogical / bsrcH) : 1.0f;
		const float bdW = bsrcW * s * scale;
		const float bdH = bannerHeightPixels; // exactly bannerHeightLogical * scale
		const float windowWidth = GlobalConstant::SCREEN_WIDTH * scale;
		const float bdX = (windowWidth - bdW) * 0.5f; // center horizontally
		const float bdY = 0.0f;
		const Rectangle bdst{ bdX, bdY, bdW, bdH };
		DrawTexturePro(*_banner, bsrc, bdst, pos, 0.0f, WHITE);
	}

	// Draw level indicator text to the left of the banner in the top strip
	if (_level >= 0)
	{
		// const std::string levelText = "Level: " + std::to_string(level);
		// const int textY = static_cast<int>(GlobalConstant::QUARTER * GlobalConstant::BASE_SIZE); // Using GlobalConstant::QUARTER constant
		// ds.DrawText(levelText, 10, textY, textY, WHITE);

		// Choose a readable font size that fits within the banner strip
		const int fontSize = 50;

		const std::string text = "Level: " + std::to_string(_level);
		// int textWidth = MeasureText(text.c_str(), fontSize);
		const float padding = GlobalConstant::BANNER_PADDING; // small left padding from globals
		float drawX = padding;
		// Ensure it doesn't overlap the banner if banner is present
		// if (bannerDrawW > 0.0f)
		// {
		// 	// Keep text fully to the left of banner
		// 	float maxTextX = bannerDrawX - static_cast<float>(textWidth) - padding;
		// 	if (maxTextX > drawX) drawX = maxTextX; // right-align against banner edge if room
		// }
		const float drawY = (bannerHeightPixels - static_cast<float>(fontSize)) * 0.5f; // vertically centered in strip
		DrawText(text.c_str(), static_cast<int>(drawX), static_cast<int>(drawY), fontSize, WHITE);
	}

	// Draw power-up bar UI in the banner strip (right-aligned, vertically centered)
	if (_powerBarTex && _powerBarTex->id > 0 && _powerBarFraction > 0.0f)
	{
		const float padding = GlobalConstant::BANNER_PADDING;
		const float texW = static_cast<float>(_powerBarTex->width);
		// const float texH = static_cast<float>(_powerBarTex->height);
		// Source: two rows: top is frame, bottom is fill; both height = BASE_SIZE
		const float rowH = GlobalConstant::BASE_SIZE;
		// Render at half the banner height
		const float logicalH = GlobalConstant::BANNER_HEIGHT * 0.5f;
		const float scaleH = logicalH / rowH; // scale to fill banner height
		const float drawH = logicalH * scale; // screen pixels
		const float frameWLogical = texW; // entire width for frame source row
		const float fillLeftLogical = 0.125f * GlobalConstant::BASE_SIZE;
		const float fillRightPadding = 0.25f * GlobalConstant::BASE_SIZE;
		const float maxFillLogical = frameWLogical - fillLeftLogical - fillRightPadding;
		const float fillWLogical = std::max(0.0f, std::min(maxFillLogical, maxFillLogical * _powerBarFraction));

		// Destination width scaled to banner height
		const float destFrameW = frameWLogical * scaleH * scale;
		const float windowWidth = GlobalConstant::SCREEN_WIDTH * scale;
		const float destX = windowWidth - padding - destFrameW; // right-aligned
	const float destY = (bannerHeightPixels - drawH) * 0.5f; // vertical center within banner

		// Draw frame (top row)
		const Rectangle srcFrame{ 0.0f, 0.0f, frameWLogical, rowH };
		const Rectangle dstFrame{ destX, destY, destFrameW, drawH };
		DrawTexturePro(*_powerBarTex, srcFrame, dstFrame, pos, 0.0f, WHITE);

		// Draw fill (bottom row), with source width based on fraction
		const Rectangle srcFill{ fillLeftLogical, rowH, fillWLogical, rowH };
		// Fill destination aligned to frame's inner left, scaled
		const float destFillX = destX + (fillLeftLogical * scaleH * scale);
		const float destFillW = fillWLogical * scaleH * scale;
		const Rectangle dstFill{ destFillX, destY, destFillW, drawH };
		DrawTexturePro(*_powerBarTex, srcFill, dstFill, pos, 0.0f, _powerBarColor);
	}

	// 2) Draw gameplay backbuffer below the banner strip
	const float destX = 0.0f;
	const float destY = bannerHeightPixels; // offset by banner height
	const Rectangle dest{ destX, destY, textureWidth * scale, textureHeight * scale };
	DrawTexturePro(_backbuffer.texture, source, dest, pos, 0.0F, WHITE);
	EndDrawing();
}
