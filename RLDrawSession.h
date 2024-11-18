#pragma once
#include <raylib.h>
#include <string>

namespace raylib
{
	class DrawSession
	{
	public:
		DrawSession(RenderTexture2D& backbuffer);
		DrawSession(RenderTexture2D& backbuffer, Color clearColor);

		~DrawSession();

		// move/copy construct
		DrawSession(DrawSession&& b) = delete;
		DrawSession(DrawSession& b) = delete;
		DrawSession& operator=(DrawSession& b) = delete;
		DrawSession& operator=(DrawSession&& b) = delete;

		void DrawRectangle(int x, int y, int width, int height, Color color);
		void DrawText(const std::string& text, int x, int y, int fontSize, Color color);
		void DrawFPS(int posX, int posY, int height);
		void DrawTexture(Texture2D& texture, int x, int y, Color color);
		void DrawTexture(Texture2D& texture, float x, float y, float width, float height, Vector2& pos, Color color);
		void DrawTexture(Texture2D& texture, Rectangle& source, Vector2& pos, Color color);
	};
} // namespace raylib
