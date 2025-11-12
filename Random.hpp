
#pragma once
#include <cstdint>
#include <xoshiro.h>

class Random
{
  public:
    // Seed with a default value or allow user to set
    static void seed(uint64_t s);
    static auto next() -> uint64_t;
    static auto uniform_int(int min, int max) -> int;
    static auto uniform_float(float min, float max) -> float;

  private:
    // Use xso::xoshiro_4x64 as the state type from xoshiro.h
    static xso::xoshiro_4x64 state;
};
