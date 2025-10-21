#pragma once
#include <raylib.h>
#include <string>

namespace raylib {
class Texture2DFile {
public:
    Texture2DFile() = default;
    explicit Texture2DFile(const std::string& filename) noexcept { load(filename); }
    ~Texture2DFile() { unload(); }

    Texture2DFile(const Texture2DFile&) = delete;
    Texture2DFile& operator=(const Texture2DFile&) = delete;
    Texture2DFile(Texture2DFile&& other) noexcept { _tex = other._tex; other._tex = {}; }
    Texture2DFile& operator=(Texture2DFile&& other) noexcept {
        if (this != &other) { unload(); _tex = other._tex; other._tex = {}; }
        return *this;
    }

    void load(const std::string& filename) noexcept { unload(); _tex = ::LoadTexture(filename.c_str()); }
    void unload() noexcept { if (_tex.id > 0) ::UnloadTexture(_tex); _tex = {}; }

    void setPointFiltering() noexcept { if (_tex.id > 0) ::SetTextureFilter(_tex, TEXTURE_FILTER_POINT); }

    [[nodiscard]] const Texture2D& get() const noexcept { return _tex; }
    [[nodiscard]] int id() const noexcept { return _tex.id; }
    [[nodiscard]] int width() const noexcept { return _tex.width; }
    [[nodiscard]] int height() const noexcept { return _tex.height; }

private:
    Texture2D _tex{};
};

class Texture2DFromImage {
public:
    Texture2DFromImage() = default;
    explicit Texture2DFromImage(const Image& image) noexcept { load(image); }
    ~Texture2DFromImage() { unload(); }

    Texture2DFromImage(const Texture2DFromImage&) = delete;
    Texture2DFromImage& operator=(const Texture2DFromImage&) = delete;
    Texture2DFromImage(Texture2DFromImage&& other) noexcept { _tex = other._tex; other._tex = {}; }
    Texture2DFromImage& operator=(Texture2DFromImage&& other) noexcept {
        if (this != &other) { unload(); _tex = other._tex; other._tex = {}; }
        return *this;
    }

    void load(const Image& image) noexcept { unload(); _tex = ::LoadTextureFromImage(image); }
    void unload() noexcept { if (_tex.id > 0) ::UnloadTexture(_tex); _tex = {}; }
    void update(const void* pixels) noexcept { if (_tex.id > 0) ::UpdateTexture(_tex, pixels); }

    void setPointFiltering() noexcept { if (_tex.id > 0) ::SetTextureFilter(_tex, TEXTURE_FILTER_POINT); }

    [[nodiscard]] const Texture2D& get() const noexcept { return _tex; }
    [[nodiscard]] int id() const noexcept { return _tex.id; }
    [[nodiscard]] int width() const noexcept { return _tex.width; }
    [[nodiscard]] int height() const noexcept { return _tex.height; }

private:
    Texture2D _tex{};
};
} // namespace raylib
