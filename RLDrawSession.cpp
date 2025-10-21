// Corresponding header
#include "RLDrawSession.h"

// Standard library headers
#include <format>
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "RLRenderTexture2D.hpp"
namespace raylib
{
    DrawSession::DrawSession(Color clearColor) noexcept
    {
        _textureMode = false;
        BeginDrawing();
        ClearBackground(clearColor);
    }
    DrawSession::DrawSession(const raylib::RenderTexture2D& backbuffer, const Color clearColor) noexcept
    {
        _textureMode = true;
        BeginTextureMode(backbuffer.get());

        ClearBackground(clearColor);
    }

    DrawSession::DrawSession(const raylib::RenderTexture2D& backbuffer) noexcept
    {
        _textureMode = true;
        BeginTextureMode(backbuffer.get());
    }

    DrawSession::~DrawSession()
    {
        if (_textureMode)
        {
            EndTextureMode();
        }
        else
        {
            EndDrawing();
        }
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
    }

    void DrawSession::DrawTexturePro(const Texture2D &texture, const Rectangle &source, const Rectangle &dest,
                                     const Vector2 &origin, const float scale, const Color color) noexcept
    {
        // Build destination rectangle scaled by the provided factor
        Rectangle scaledDest{ dest.x, dest.y, dest.width * scale, dest.height * scale };
        ::DrawTexturePro(texture, source, scaledDest, origin, 0.0f, color);
    }
    
    void DrawSession::DrawTextureEx(const Texture2D &texture, const Vector2 &pos, const float scale,
                                    const Color color) noexcept
    {
        ::DrawTextureEx(texture, pos, 0.0f, scale, color);
    }
    
    void DrawSession::DrawTextureEx(const Texture2D &texture, const float x, const float y, const float scale,
                                    const Color color) noexcept
    {
        ::DrawTextureEx(texture, Vector2{ x, y }, 0.0f, scale, color);
    }
    
    void DrawSession::DrawText(const std::string& text, const int x, const int y, const int fontSize, const Color color) noexcept
    {
        ::DrawText(text.c_str(), x, y, fontSize, color);
    }
    
    void DrawSession::DrawTextCentered(const std::string& text, const int x, const int y, const int fontSize, const Color color) noexcept
    {
        const int textWidth = MeasureText(text.c_str(), fontSize);
        const int drawX = x - textWidth / 2;
        const int drawY = y - fontSize / 2;
        ::DrawText(text.c_str(), drawX, drawY, fontSize, color);
    }
    
    void DrawSession::DrawFPS(const int posX, const int posY, const int height)
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

    void DrawSession::DrawCircle(const float centerX, const float centerY, const float radius, const Color color) noexcept
    {
        ::DrawCircle(static_cast<int>(centerX), static_cast<int>(centerY), radius, color);
    }
} // namespace raylib
