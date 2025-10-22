#pragma once
#include <raylib.h>

#include "Global.hpp"

namespace raylib {
class RenderTexture2D
{
public:
    RenderTexture2D() = default;
    RenderTexture2D(GameTypes::Coordinate w, GameTypes::Coordinate h) noexcept { create(w, h); }
    ~RenderTexture2D() { destroy(); }

    RenderTexture2D(const RenderTexture2D&) = delete;
    RenderTexture2D& operator=(const RenderTexture2D&) = delete;
    RenderTexture2D(RenderTexture2D&& other) noexcept { _rt = other._rt; other._rt = {}; }
    RenderTexture2D& operator=(RenderTexture2D&& other) noexcept {
        if (this != &other) { destroy(); _rt = other._rt; other._rt = {}; }
        return *this;
    }

    void create(GameTypes::Coordinate w, GameTypes::Coordinate h) noexcept { destroy(); _rt = ::LoadRenderTexture(w, h); }
    void destroy() noexcept { if (_rt.id > 0) { ::UnloadRenderTexture(_rt); } _rt = {}; }

    [[nodiscard]] const ::RenderTexture2D& get() const noexcept { return _rt; }
    [[nodiscard]] ::RenderTexture2D& get() noexcept { return _rt; }
    [[nodiscard]] const Texture2D& texture() const noexcept { return _rt.texture; }
    [[nodiscard]] int width() const noexcept { return _rt.texture.width; }
    [[nodiscard]] float widthF() const noexcept { return static_cast<float>(_rt.texture.width); }
    [[nodiscard]] int height() const noexcept { return _rt.texture.height; }
    [[nodiscard]] float heightF() const noexcept { return static_cast<float>(_rt.texture.height); }

private:
    ::RenderTexture2D _rt{};
};
} // namespace raylib
