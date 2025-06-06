#include <string>
#include <raylib.h>

#include "RLWindow.h"
namespace raylib
{
    Window::Window(const uint16_t width, const uint16_t height, const uint16_t fps, const std::string& title)
    {
#ifndef _DEBUG
        SetTraceLogLevel(LOG_NONE);
#endif // !DEBUG
        InitWindow(width, height, title.c_str());
        SetTargetFPS(fps);
        InitAudioDevice();
    }

    Window::~Window()
    {
        CloseAudioDevice();
        CloseWindow();
    }

    bool Window::ShouldClose() const noexcept
    {
        return WindowShouldClose();
    }
} // namespace raylib

