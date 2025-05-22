#include <string>
#include <format>

#include <raylib.h>

#include "RLDrawSession.h"
namespace raylib
{
    DrawSession::DrawSession(const RenderTexture2D& backbuffer, const Color clearColor) noexcept
    {
        BeginTextureMode(backbuffer);

        ClearBackground(clearColor);
    }

    DrawSession::DrawSession(const RenderTexture2D& backbuffer) noexcept
    {
        BeginTextureMode(backbuffer);
    }

    DrawSession::~DrawSession()
    {
        EndTextureMode();
    }    void DrawSession::DrawRectangle(const int x, const int y, const int width, const int height, const Color color) noexcept
    {
        ::DrawRectangle(x, y, width, height, color);
    }

    void DrawSession::DrawTexture(const Texture2D& texture, const int x, const int y, const Color color) noexcept
	{
		::DrawTexture(texture, x, y, color);
	}

    void DrawSession::DrawTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, const Vector2& pos, const Color color) noexcept
    {
        ::DrawTextureRec(texture, { x,y,width,height }, pos, color);
    }

    void DrawSession::DrawTexture(const Texture2D& texture, const Rectangle& source, const Vector2& pos, const Color color) noexcept
    {
        ::DrawTextureRec(texture, source, pos, color);
    }    void DrawSession::DrawText(const std::string& text, const int x, const int y, const int fontSize, const Color color) noexcept
    {
        ::DrawText(text.c_str(), x, y, fontSize, color);
    }    void DrawSession::DrawTextCentered(const std::string& text, const int x, const int y, const int fontSize, const Color color) noexcept
    {
        const int textWidth = MeasureText(text.c_str(), fontSize);
        const float drawX = static_cast<float>(x) - textWidth / 2.0f;
        const float drawY = static_cast<float>(y) - fontSize / 2.0f;
        ::DrawText(text.c_str(), static_cast<int>(drawX), static_cast<int>(drawY), fontSize, color);
    }    void DrawSession::DrawFPS(const int posX, const int posY, const int height)
    {
        Color color = LIME;
        const int fps = GetFPS();

        if ((fps < 60) && (fps >= 30))
        {
            color = ORANGE;
        }
        else if (fps < 30)
        {
            color = RED;
        }

        const std::string fpsStr = std::format("FPS: {}", fps); // Use local fps instead of calling GetFPS() again
        DrawText(fpsStr, posX, posY, height, color);
    }
} // namespace raylib
