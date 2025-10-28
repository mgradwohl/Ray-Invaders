#pragma once

// Third-party headers
#include <raylib.h>

// Project-local, constexpr inline AABB intersection.
// Returns true if rectangles overlap with positive area (edge-touching is not a collision),
// mirroring CheckCollisionRecs semantics.
static constexpr inline auto AabbIntersect(const Rectangle &a, const Rectangle &b) noexcept -> bool
{
    return a.x + a.width > b.x && b.x + b.width > a.x && a.y + a.height > b.y && b.y + b.height > a.y;
}
