// Minimal xoshiro256++ implementation for integration
// Source: https://github.com/lemire/xoshiro256plusplus (MIT License)
#pragma once
#include <stdint.h>

struct xoshiro256plusplus_state {
    uint64_t s[4];
};

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static inline uint64_t xoshiro256plusplus_next(xoshiro256plusplus_state* state) {
    const uint64_t result = rotl(state->s[0] + state->s[3], 23) + state->s[0];
    const uint64_t t = state->s[1] << 17;

    state->s[2] ^= state->s[0];
    state->s[3] ^= state->s[1];
    state->s[1] ^= state->s[2];
    state->s[0] ^= state->s[3];

    state->s[2] ^= t;
    state->s[3] = rotl(state->s[3], 45);

    return result;
}
