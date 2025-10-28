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
inline auto alpha_from_ttl(int ttl) noexcept -> unsigned char
{
    const int t = std::clamp(ttl, 0, TTL_WINDOW_DEFAULT);
    return static_cast<unsigned char>(50 + (205 * t) / TTL_WINDOW_DEFAULT);
}

// TTL presets per subject/outcome
inline auto default_ttl(HitSubject subject, HitOutcome outcome) noexcept -> int
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

inline auto style_for(HitSubject subject, HitOutcome outcome, unsigned char alpha) noexcept -> Style
{
    // TODO refactor this to just set colors in each case and then return at the end

    switch (subject)
    {
    case HitSubject::Base:
        return Style{GlobalColors::withAlpha(GlobalColors::HIT_BASE_CORE, alpha),
                     GlobalColors::withAlpha(GlobalColors::HIT_BASE_OUTLINE, alpha)};
    case HitSubject::Enemy:
        return Style{GlobalColors::withAlpha(GlobalColors::HIT_ENEMY_CORE, alpha),
                     GlobalColors::withAlpha(GlobalColors::HIT_ENEMY_OUTLINE, alpha)};
    case HitSubject::Player:
        if (outcome == HitOutcome::Destroyed)
        {
            return Style{GlobalColors::withAlpha(GlobalColors::HIT_PLAYER_DESTROYED_CORE, alpha),
                         GlobalColors::withAlpha(GlobalColors::HIT_PLAYER_DESTROYED_OUTLINE, alpha)};
        }
        return Style{GlobalColors::withAlpha(GlobalColors::HIT_PLAYER_NONFATAL_CORE, alpha),
                     GlobalColors::withAlpha(GlobalColors::HIT_PLAYER_NONFATAL_OUTLINE, alpha)};
    case HitSubject::Ufo:
        return Style{GlobalColors::withAlpha(GlobalColors::HIT_UFO_CORE, alpha),
                     GlobalColors::withAlpha(GlobalColors::HIT_UFO_OUTLINE, alpha)};
    }
    return Style{GlobalColors::withAlpha(WHITE, alpha), GlobalColors::withAlpha(WHITE, alpha)};
}

// Radius presets per subject/outcome
inline auto default_radius(HitSubject subject, HitOutcome outcome) noexcept -> float
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
        {
            it = _hits.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void HitManager::draw(raylib::DrawSession &ds) const
{
    for (const auto &hit : _hits)
    {
        const unsigned char a = alpha_from_ttl(hit.ttl);
        const Style s = style_for(hit.subject, hit.outcome, a);
        ds.DrawCircle(hit.x, hit.y, hit.radius, s.core);
        ds.DrawCircle(hit.x, hit.y, hit.radius + OUTLINE_DELTA, s.outline);
    }
}
