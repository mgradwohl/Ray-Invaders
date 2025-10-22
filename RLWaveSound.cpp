// Corresponding header
#include "RLWaveSound.hpp"

// Standard library headers
#include <string>

// Third-party headers
#include <raylib.h>

namespace raylib
{
WaveSound::WaveSound(const std::string &filename) noexcept { _sound = LoadSound(filename.c_str()); }

WaveSound::~WaveSound()
{
    if (::IsSoundValid(_sound))
    {
        StopSound(_sound);
        UnloadSound(_sound);
    }
}

bool WaveSound::Play() const noexcept
{
    if (!IsSoundValid(_sound))
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

bool WaveSound::IsPlaying() const noexcept { return IsSoundPlaying(_sound); }
} // namespace raylib