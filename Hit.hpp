#pragma once
#include <cstdint>
#include <raylib.h>

enum class HitSubject : std::uint8_t
{
    Enemy,
    Player,
    Ufo,
    Base
};

enum class HitOutcome : std::uint8_t
{
    NonFatal,
    Destroyed
};

struct Hit
{
    static constexpr float DEFAULT_RADIUS = 2.5F;
    static constexpr int DEFAULT_TTL = 60;

    float x{0.0F};
    float y{0.0F};
    float radius{DEFAULT_RADIUS};
    int ttl{DEFAULT_TTL};
    HitSubject subject{HitSubject::Enemy};
    HitOutcome outcome{HitOutcome::NonFatal};

    // Convenience constructor to allow in-place construction in vectors
    constexpr Hit(float ix, float iy, float iradius, int ittl, HitSubject isubject, HitOutcome ioutcome) noexcept
        : x(ix), y(iy), radius(iradius), ttl(ittl), subject(isubject), outcome(ioutcome)
    {
    }
};
