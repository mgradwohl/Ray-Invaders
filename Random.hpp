
#pragma once
#include <cstdint>
extern "C++" {
#include "xoshiro.h"
}

class Random {
public:
    // Seed with a default value or allow user to set
    static void seed(uint64_t s);
    static uint64_t next();
    static int uniform_int(int min, int max);
    static float uniform_float(float min, float max);
private:
    // Use xso::xoshiro_4x64 as the state type from xoshiro.h
    static xso::xoshiro_4x64 state;
};
