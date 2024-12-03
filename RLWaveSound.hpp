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

		bool Play() const noexcept;
		void Stop() const noexcept;

		bool IsPlaying() const noexcept;

	private:
		Sound _sound;
	};
}

