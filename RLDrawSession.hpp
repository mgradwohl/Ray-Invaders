#pragma once
#include <string>

#include <raylib.h>

// Forward declaration
namespace raylib
{
class RenderTexture2D;
}

namespace raylib
{
	class DrawSession
	{
	public:
    // Screen session: BeginDrawing/EndDrawing
        explicit DrawSession(Color clearColor) noexcept;
    DrawSession(const raylib::RenderTexture2D &backbuffer) noexcept;
        DrawSession(const raylib::RenderTexture2D &backbuffer, Color clearColor) noexcept;

        ~DrawSession();

		// move/copy construct
    DrawSession(DrawSession &&b) = delete;
    DrawSession(DrawSession &b) = delete;
    auto operator=(DrawSession &b) -> DrawSession & = delete;
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
        // Wrapper for raylib::DrawTexturePro without rotation; rotation forced to 0
        void DrawTexturePro(const Texture2D &texture, const Rectangle &source, const Rectangle &dest,
                    const Vector2 &origin, float scale, Color color) noexcept;
        // Wrapper for raylib::DrawTextureEx to allow scaling when drawing textures (no rotation)
        void DrawTextureEx(const Texture2D &texture, const Vector2 &pos, float scale,
                    Color color) noexcept;
        // Convenience overload using scalar position instead of Vector2 (no rotation)
        void DrawTextureEx(const Texture2D &texture, float x, float y, float scale,
               Color color) noexcept;
        void DrawCircle(float centerX, float centerY, float radius, Color color) noexcept;

    private:
        bool _textureMode{false};
    };
} // namespace raylib
