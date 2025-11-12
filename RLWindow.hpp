#pragma once
#include <cstdint>
#include <string>

#include <raylib.h>

namespace raylib
{
class Window
{
  public:
    Window(uint16_t width, uint16_t height, uint16_t fps, const std::string &title = "raylib::Window", const std::string &icon = "");
    Window() = delete;
    ~Window();

    // move/copy constuct
    Window(Window &&b) = delete;
    Window(Window &b) = delete;
    auto operator=(Window &b) -> Window & = delete;
    auto operator=(Window &&b) -> Window & = delete;

    [[nodiscard]] static auto ShouldClose() noexcept -> bool;

  private:
    Image imageIcon;
};
} // namespace raylib
