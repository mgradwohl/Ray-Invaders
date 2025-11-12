// Corresponding header
#include "RLWindow.hpp"

// Standard library headers
#include <cstdint>
#include <iostream>
#include <string>

// Third-party headers
#include <raylib.h>
namespace raylib
{

Window::Window(const uint16_t width, const uint16_t height, const uint16_t fps, const std::string &title, const std::string &icon)
    : imageIcon{} // initialize imageIcon

{
    SetTraceLogLevel(LOG_ERROR);
    std::cerr << "Window::Window\n";
    InitWindow(width, height, title.c_str());
    imageIcon = LoadImage(icon.c_str());
    if (IsImageValid(imageIcon))
    {
        SetWindowIcon(imageIcon);
    }
    else
    {
        std::cerr << "Warning: Image Icon not valid\n";
    }
    SetTargetFPS(fps);
    InitAudioDevice();
    if (!IsAudioDeviceReady())
    {
        std::cerr << "Warning: Audio device not ready after InitAudioDevice()\n";
    }
    SetMasterVolume(1.0F);
    if (!IsAudioDeviceReady())
    {
        std::cerr << "Warning: Audio device is not ready. Sounds may not play.\n";
    }
}

Window::~Window()

{
    UnloadImage(imageIcon);
    CloseAudioDevice();
    CloseWindow();
}

auto Window::ShouldClose() noexcept -> bool

{
    return WindowShouldClose();
}
} // namespace raylib
