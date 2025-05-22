#pragma once
#include <raylib.h>
#include <string>

namespace raylib
{
	class DrawSession
	{
	public:
		DrawSession(const RenderTexture2D& backbuffer) noexcept;
		DrawSession(const RenderTexture2D& backbuffer, const Color clearColor) noexcept;

		~DrawSession();

		// move/copy construct
		DrawSession(DrawSession&& b) = delete;
		DrawSession(DrawSession& b) = delete;
		DrawSession& operator=(DrawSession& b) = delete;
		DrawSession& operator=(DrawSession&& b) = delete;

	void DrawRectangle(const int x, const int y, const int width, const int height, const Color color) noexcept;
	void DrawText(const std::string& text, const int x, const int y, const int fontSize, const Color color) noexcept;
	void DrawTextCentered(const std::string& text, const int x, const int y, const int fontSize, const Color color) noexcept;
	void DrawFPS(const int posX, const int posY, const int height);
	void DrawTexture(const Texture2D& texture, const int x, const int y, const Color color) noexcept;
	void DrawTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, const Vector2& pos, const Color color) noexcept;
	void DrawTexture(const Texture2D& texture, const Rectangle& source, const Vector2& pos, const Color color) noexcept;
	};
} // namespace raylib
