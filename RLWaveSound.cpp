// Corresponding header
#include "RLWaveSound.hpp"

// Standard library headers
#include <iostream>
#include <string>

// Third-party headers
#include <raylib.h>

namespace raylib
{
WaveSound::WaveSound(const std::string &filename) noexcept
{
    // First, try the simple path
    _sound = LoadSound(filename.c_str());
    if (!::IsSoundValid(_sound))
    {
    // Surface a clear warning that this specific sound failed to load
    std::cerr << "Warning: Failed to load sound: " << filename << '\n';
    }

    if (!::IsSoundValid(_sound))
    {
        // Fallback: Load the wave and convert to a widely-supported format (16-bit PCM)
        Wave wave = LoadWave(filename.c_str());
        if (wave.frameCount > 0 && wave.data != nullptr)
        {
            // Preserve original sample rate and channels, promote to 16-bit
            WaveFormat(&wave, wave.sampleRate, 16, wave.channels);
            _sound = LoadSoundFromWave(wave);
            UnloadWave(wave);
        }

        if (!::IsSoundValid(_sound))
        {
            std::cerr << "Warning: Failed to load sound (unsupported or unreadable): " << filename << '\n';
        }
    }
}

WaveSound::~WaveSound()
{
    if (::IsSoundValid(_sound))
    {
        StopSound(_sound);
        UnloadSound(_sound);
    }
}

auto WaveSound::Play() const noexcept -> bool
{
    if (!::IsSoundValid(_sound))
    {
        return false;
    }

    PlaySound(_sound);
    return true;
}
void WaveSound::Stop() const noexcept
{
    if (::IsSoundValid(_sound))
    {
        StopSound(_sound);
    }
}

auto WaveSound::IsPlaying() const noexcept -> bool
{
    return ::IsSoundPlaying(_sound);
}
} // namespace raylib