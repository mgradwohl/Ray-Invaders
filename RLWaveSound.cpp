
#include <string>
#include <raylib.h>
#include "RLWaveSound.hpp"

namespace raylib
{
    WaveSound::WaveSound(const std::string& filename) noexcept
    {
        _sound = LoadSound(filename.c_str());
    }

    WaveSound::~WaveSound()
    {
        StopSound(_sound);
        //UnloadSound(sound);
    }

    bool WaveSound::Play() const noexcept
    {
#ifdef DEBUG
        if (!IsSoundValid(sound))
        {
            return false;
        }
#endif
        
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

    bool WaveSound::IsPlaying() const noexcept
    {
        return IsSoundPlaying(_sound);
    }
} // namespace raylib