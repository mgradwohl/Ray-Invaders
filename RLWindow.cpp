// Corresponding header
#include "RLWindow.h"

// Standard library headers
#include <string>

// Third-party headers
#include <raylib.h>
namespace raylib
{
Window::Window(const uint16_t width, const uint16_t height, const uint16_t fps,
               const std::string &title)
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, title.c_str());
    SetTargetFPS(fps);
    InitAudioDevice();
}

Window::~Window()
{
    CloseAudioDevice();
    CloseWindow();
}

bool Window::ShouldClose() const noexcept { return WindowShouldClose(); }
} // namespace raylib
