#pragma once
#include <string>

#include <raylib.h>
namespace raylib
{
class WaveSound
{
  public:
    WaveSound() = default;
    WaveSound(const std::string &filename) noexcept;
    ~WaveSound();

    // Non-copyable / non-movable to avoid double-free of owned resource
    WaveSound(const WaveSound &) = delete;
    WaveSound &operator=(const WaveSound &) = delete;
    WaveSound(WaveSound &&) = delete;
    WaveSound &operator=(WaveSound &&) = delete;

    [[nodiscard]] auto Play() const noexcept -> bool;
    void Stop() const noexcept;

    [[nodiscard]] auto IsPlaying() const noexcept -> bool;

  private:
    Sound _sound{};
};
} // namespace raylib
