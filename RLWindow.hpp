#pragma once
#include <string>
#include <cstdint>

namespace raylib
{
	class Window
	{
	public:
      Window(uint16_t width, uint16_t height, uint16_t fps,
             const std::string &title = "raylib::Window");
      Window() = delete;
      ~Window();

      // move/copy constuct
      Window(Window &&b)                     = delete;
      Window(Window &b)                      = delete;
      auto operator=(Window &b) -> Window &  = delete;
      auto operator=(Window &&b) -> Window & = delete;

      [[nodiscard]] auto ShouldClose() const noexcept -> bool;
    };
} // namespace raylib
