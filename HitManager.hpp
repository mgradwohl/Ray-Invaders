#pragma once
#include <vector>

#include <raylib.h>

#include "Hit.hpp"
#include "RLDrawSession.h"

class HitManager
{
public:
    // Add a hit with explicit ttl
    void add_hit(HitSubject subject, HitOutcome outcome, float x, float y, float radius, int ttl) noexcept;
    // Preset-ttl overload: uses internal defaults per subject/outcome
    void add_hit(HitSubject subject, HitOutcome outcome, float x, float y, float radius) noexcept;
    // Full preset overload: uses internal defaults for ttl and radius
    void add_hit(HitSubject subject, HitOutcome outcome, float x, float y) noexcept;
    void update() noexcept;
    void draw(raylib::DrawSession& ds) const;

private:
    std::vector<Hit> _hits;
};
