
#include <string>
#include <raylib.h>
#include "RLWaveSound.hpp"

namespace raylib
{
    WaveSound::WaveSound(const std::string& filename)
    {
        sound = LoadSound(filename.c_str());
    }

    WaveSound::~WaveSound()
    {
        StopSound(sound);
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
        
        PlaySound(sound);
        return true;
    }
    void WaveSound::Stop() const noexcept
    {
        StopSound(sound);
    }

    bool WaveSound::IsPlaying() const noexcept
    {
        return IsSoundPlaying(sound);
    }
} // namespace raylib