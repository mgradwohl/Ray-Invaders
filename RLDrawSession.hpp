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
    
    static void DrawRectangle(int x, int y, int width, int height, Color color) noexcept;
    static void DrawText(const std::string &text, int x, int y, int fontSize, Color color) noexcept;
    static void DrawTextCentered(const std::string &text, int x, int y, int fontSize, Color color) noexcept;
    static void DrawFPS(int posX, int posY, int height);
    static void DrawTexture(const Texture2D &texture, int x, int y, Color color) noexcept;
    static void DrawTexture(const Texture2D &texture, float x, float y, float width, float height, const Vector2 &pos, Color color) noexcept;
    static void DrawTexture(const Texture2D &texture, const Rectangle &source, const Vector2 &pos, Color color) noexcept;
    // Wrapper for raylib::DrawTexturePro without rotation; rotation forced to 0
    static void DrawTexturePro(const Texture2D &texture, const Rectangle &source, const Rectangle &dest, const Vector2 &origin, float scale,
                        Color color) noexcept;
    // Wrapper for raylib::DrawTextureEx to allow scaling when drawing textures (no rotation)
    static void DrawTextureEx(const Texture2D &texture, const Vector2 &pos, float scale, Color color) noexcept;
    // Convenience overload using scalar position instead of Vector2 (no rotation)
    static void DrawTextureEx(const Texture2D &texture, float x, float y, float scale, Color color) noexcept;
    static void DrawCircle(float centerX, float centerY, float radius, Color color) noexcept;

  private:
    bool _textureMode{false};
};
} // namespace raylib
