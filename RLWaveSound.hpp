#pragma once
#include <raylib.h>
#include <string>
namespace raylib
{
	class WaveSound
	{
	public:
		WaveSound() = default;
		WaveSound(const std::string& filename) noexcept;
		~WaveSound();

		// move/copy constuct
		//WaveSound(WaveSound&& b) = delete;
		//WaveSound(WaveSound& b) = delete;
		//WaveSound& operator=(WaveSound& b) = delete;
		//WaveSound& operator=(WaveSound&& b) = delete;

        [[nodiscard]] auto Play() const noexcept -> bool;
        void Stop() const noexcept;

        [[nodiscard]] auto IsPlaying() const noexcept -> bool;

      private:
		Sound _sound{};
	};
}

