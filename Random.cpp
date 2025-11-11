
#include "Random.hpp"
#include <array>
// using namespace xso;

// Default seed value for xso::xoshiro_4x64
xso::xoshiro_4x64 Random::state;

void Random::seed(uint64_t s) {
    // Use the seed method from xso::xoshiro_4x64
    std::array<uint64_t, 4> arr = {s, s ^ 0x9E3779B97F4A7C15, s ^ 0xD1B54A32D192ED03, s ^ 0x94D049BB133111EB};
    state.seed(arr.begin(), arr.end());
}

auto Random::next() -> uint64_t {
    // Use the plus scrambler for output
    uint64_t result = state[0] + state[3];
    state.step();
    return result;
}

auto Random::uniform_int(int min, int max) -> int {
    uint64_t r = next();
    return min + (r % (max - min + 1));
}

auto Random::uniform_float(float min, float max) -> float {
    uint64_t r = next();
    float norm = (r >> 11) * (1.0f / 9007199254740991.0f); // 53 bits
    return min + (norm * (max - min));
}
