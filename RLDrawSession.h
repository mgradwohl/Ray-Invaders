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

		void DrawRectangle(int x, int y, int width, int height, Color color) noexcept;
		void DrawText(const std::string& text, int x, int y, int fontSize, Color color) noexcept;
		void DrawFPS(int posX, int posY, int height);
		void DrawTexture(const Texture2D& texture, const int x, const int y, const Color color) noexcept;
		void DrawTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, const Vector2& pos, const Color color) noexcept;
		void DrawTexture(const Texture2D& texture, const Rectangle& source, const Vector2& pos, const Color color) noexcept;
	};
} // namespace raylib
