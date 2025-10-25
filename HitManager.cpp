// Corresponding header
#include "HitManager.hpp"

// Standard library
#include <algorithm>

// Third-party
#include <raylib.h>

// Project
#include "Global.hpp"

namespace
{
// Module-local tuning constants
constexpr int TTL_WINDOW_DEFAULT = 60;
constexpr int TTL_WINDOW_UFO = 75;
constexpr int TTL_PLAYER_NONFATAL = 30;

constexpr float RADIUS_ENEMY = 2.5F;
constexpr float RADIUS_BASE = 2.5F;
constexpr float RADIUS_PLAYER_NONFATAL = 3.0F;
constexpr float RADIUS_PLAYER_DESTROYED = 4.0F;
constexpr float RADIUS_UFO = 5.0F;
constexpr float OUTLINE_DELTA = 1.0F;

// Map ttl into alpha 255..50 over a TTL_WINDOW_DEFAULT-frame window
inline unsigned char alpha_from_ttl(int ttl) noexcept
{
    const int t = std::clamp(ttl, 0, TTL_WINDOW_DEFAULT);
    return static_cast<unsigned char>(50 + (205 * t) / TTL_WINDOW_DEFAULT);
}

// TTL presets per subject/outcome
inline int default_ttl(HitSubject subject, HitOutcome outcome) noexcept
{
    // Keep base fade window parity: 60 for most; UFO can be a touch longer if desired.
    switch (subject)
    {
    case HitSubject::Enemy:
        return TTL_WINDOW_DEFAULT;
    case HitSubject::Base:
        return TTL_WINDOW_DEFAULT;
    case HitSubject::Player:
        // Non-fatal (shield) is snappier; destroyed lingers a bit longer
        return (outcome == HitOutcome::NonFatal) ? TTL_PLAYER_NONFATAL : TTL_WINDOW_DEFAULT;
    case HitSubject::Ufo:
        return TTL_WINDOW_UFO; // pops a bit longer
    }
    return TTL_WINDOW_DEFAULT;
}

// Style presets: colors by subject/outcome
struct Style
{
    Color core;
    Color outline;
};

inline Style style_for(HitSubject subject, HitOutcome outcome, unsigned char a) noexcept
{
    switch (subject)
    {
    case HitSubject::Base:
        return Style{Color{200, 40, 40, a}, Color{255, 120, 120, a}};
    case HitSubject::Enemy:
        return Style{Color{220, 60, 200, a}, Color{255, 150, 230, a}};
    case HitSubject::Player:
        if (outcome == HitOutcome::Destroyed)
        {
            return Style{Color{255, 140, 0, a}, Color{255, 200, 80, a}};
        }
        return Style{Color{40, 120, 255, a}, Color{120, 180, 255, a}};
    case HitSubject::Ufo:
        return Style{Color{60, 255, 180, a}, Color{140, 255, 220, a}};
    }
    return Style{Color{255, 255, 255, a}, Color{255, 255, 255, a}};
}

// Radius presets per subject/outcome
inline float default_radius(HitSubject subject, HitOutcome outcome) noexcept
{
    switch (subject)
    {
    case HitSubject::Enemy:
        return RADIUS_ENEMY;
    case HitSubject::Base:
        return RADIUS_BASE; // actual base varies by damage; this is a reasonable visual default
    case HitSubject::Player:
        return (outcome == HitOutcome::Destroyed) ? RADIUS_PLAYER_DESTROYED : RADIUS_PLAYER_NONFATAL;
    case HitSubject::Ufo:
        return RADIUS_UFO;
    }
    return RADIUS_PLAYER_NONFATAL;
}
} // namespace

HitManager::HitManager() noexcept
{
    // Reserve to avoid frequent reallocations during bursts (explosions, shields, etc.)
    _hits.reserve(128);
}

void HitManager::add_hit(HitSubject subject, HitOutcome outcome, float x, float y, float radius, int ttl) noexcept
{
    _hits.emplace_back(x, y, radius, ttl, subject, outcome);
}

void HitManager::add_hit(HitSubject subject, HitOutcome outcome, float x, float y, float radius) noexcept
{
    _hits.emplace_back(x, y, radius, default_ttl(subject, outcome), subject, outcome);
}

void HitManager::add_hit(HitSubject subject, HitOutcome outcome, float x, float y) noexcept
{
    _hits.emplace_back(x, y, default_radius(subject, outcome), default_ttl(subject, outcome), subject, outcome);
}

void HitManager::update() noexcept
{
    for (auto it = _hits.begin(); it != _hits.end();)
    {
        it->ttl -= 1;
        if (it->ttl <= 0)
            it = _hits.erase(it);
        else
            ++it;
    }
}

void HitManager::draw(raylib::DrawSession &ds) const
{
    for (const auto &h : _hits)
    {
        const unsigned char a = alpha_from_ttl(h.ttl);
        const Style s = style_for(h.subject, h.outcome, a);
        ds.DrawCircle(h.x, h.y, h.radius, s.core);
        ds.DrawCircle(h.x, h.y, h.radius + OUTLINE_DELTA, s.outline);
    }
}
