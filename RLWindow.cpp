// Corresponding header
#include "RLWindow.hpp"

// Standard library headers
#include <cstdio>
#include <string>

// Third-party headers
#include <raylib.h>
namespace raylib
{
Window::Window(const uint16_t width, const uint16_t height, const uint16_t fps, const std::string &title)
{
    // Keep error-level logs visible for diagnostics (avoid full spam in release)
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(width, height, title.c_str());
    SetTargetFPS(fps);
    InitAudioDevice();
    if (!IsAudioDeviceReady())
    {
        // Minimal diagnostic to help trace audio init issues at runtime
        fprintf(stderr, "Warning: Audio device not ready after InitAudioDevice()\n");
    }
    SetMasterVolume(1.0F);
    if (!IsAudioDeviceReady())
    {
        // Helpful runtime hint when audio backend initialization fails
        fprintf(stderr, "Warning: Audio device is not ready. Sounds may not play.\n");
    }
}

Window::~Window()
{
    CloseAudioDevice();
    CloseWindow();
}

bool Window::ShouldClose() const noexcept { return WindowShouldClose(); }
} // namespace raylib
