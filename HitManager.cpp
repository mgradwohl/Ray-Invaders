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
// Map ttl into alpha 255..50 over a 60-frame window
inline unsigned char alpha_from_ttl(int ttl) noexcept
{
    const int window = 60;
    const int t = std::clamp(ttl, 0, window);
    return static_cast<unsigned char>(50 + (205 * t) / window);
}
// TTL presets per subject/outcome
inline int default_ttl(HitSubject subject, HitOutcome outcome) noexcept
{
    // Keep base fade window parity: 60 for most; UFO can be a touch longer if desired.
    switch (subject)
    {
    case HitSubject::Enemy:
        return 60;
    case HitSubject::Base:
        return 60;
    case HitSubject::Player:
        // Non-fatal (shield) is snappier; destroyed lingers a bit longer
        return (outcome == HitOutcome::NonFatal) ? 30 : 60;
    case HitSubject::Ufo:
        return 75; // pops a bit longer
    }
    return 60;
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
        return 2.5f;
    case HitSubject::Base:
        return 2.5f; // actual base varies by damage; this is a reasonable visual default
    case HitSubject::Player:
        return (outcome == HitOutcome::Destroyed) ? 4.0f : 3.0f;
    case HitSubject::Ufo:
        return 5.0f;
    }
    return 3.0f;
}
} // namespace

void HitManager::add_hit(HitSubject subject, HitOutcome outcome, float x, float y, float radius,
                         int ttl) noexcept
{
    _hits.push_back(Hit{x, y, radius, ttl, subject, outcome});
}

void HitManager::add_hit(HitSubject subject, HitOutcome outcome, float x, float y,
                         float radius) noexcept
{
    _hits.push_back(Hit{x, y, radius, default_ttl(subject, outcome), subject, outcome});
}

void HitManager::add_hit(HitSubject subject, HitOutcome outcome, float x, float y) noexcept
{
    _hits.push_back(Hit{x, y, default_radius(subject, outcome), default_ttl(subject, outcome),
                        subject, outcome});
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
        ds.DrawCircle(h.x, h.y, h.radius + 1.0f, s.outline);
    }
}
