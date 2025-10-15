#pragma once
#include <raylib.h>
#include <string>

namespace raylib
{
	class DrawSession
	{
	public:
		DrawSession(const RenderTexture2D& backbuffer) noexcept;
        DrawSession(const RenderTexture2D &backbuffer, Color clearColor) noexcept;

        ~DrawSession();

		// move/copy construct
		DrawSession(DrawSession&& b) = delete;
		DrawSession(DrawSession& b) = delete;
        auto operator=(DrawSession &b) -> DrawSession &  = delete;
        auto operator=(DrawSession &&b) -> DrawSession & = delete;
        void DrawRectangle(int x, int y, int width, int height, Color color) noexcept;
        void DrawText(const std::string &text, int x, int y, int fontSize, Color color) noexcept;
        void DrawTextCentered(const std::string &text, int x, int y, int fontSize,
                              Color color) noexcept;
        void DrawFPS(int posX, int posY, int height);
        void DrawTexture(const Texture2D &texture, int x, int y, Color color) noexcept;
        void DrawTexture(const Texture2D &texture, float x, float y, float width, float height,
                         const Vector2 &pos, Color color) noexcept;
        void DrawTexture(const Texture2D &texture, const Rectangle &source, const Vector2 &pos,
                         Color color) noexcept;
        void DrawCircle(float centerX, float centerY, float radius, Color color) noexcept;
    };
} // namespace raylib
