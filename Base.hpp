#pragma once

// Standard library headers
#include <vector>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Bullet.hpp"
#include "Global.hpp"
#include "RLDrawSession.h"

class Base {
public:
    Base() = default;
    explicit Base(float x) noexcept;
    ~Base();
    // Disable copying since we have a texture resource
    Base(const Base&) = delete;
    auto operator=(const Base&) -> Base& = delete;
    // Move operations need to transfer ownership of resources
    Base(Base&&) noexcept;
    auto operator=(Base&&) noexcept -> Base&;

    void reset(const Image& baseImage) noexcept;
    void update(std::vector<Bullet>& i_bullets, GameTypes::Count framecount);
    void draw(raylib::DrawSession& ds) const;
    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    [[nodiscard]] auto is_dead() const noexcept -> bool { return _dead; }
    // Change return type to int to match _damage type and avoid casting
    [[nodiscard]] auto get_damage() const noexcept -> int { return static_cast<int>(_damage); }
    
    // Records a bullet impact at the specified position and modifies the texture
    void apply_impact(float rel_x, float rel_y, float damage_amount);
    // Visual markers to show impact points for a few frames
    struct ImpactMarker { float x; float y; int ttl; };
    // Impact entries for draw-time rendering (do not mutate texture)
    struct Impact { float x; float y; float radius; int ttl; };

private:
    // Impacts recorded for draw-time rendering
    std::vector<Impact> _impacts{};
    
    Texture2D _texture{}; // Each base has its own texture
    // CPU-side damage mask (1==intact white, 0==destroyed black) and GPU texture
    // This allows pixel-precise mutation and sampling for hit-testing.
    Image _damage_image{};
    Texture2D _damage_tex{};
    // Alpha mask of the base sprite so we only apply damage to visible pixels
    Image _base_alpha{};
    // Mark that damage texture needs GPU update (to avoid frequent uploads)
    mutable bool _damage_gpu_dirty{false};
    // Per-pixel boolean mask (1 = opaque/visible, 0 = transparent) derived from the base sprite
    std::vector<unsigned char> _base_mask{};
    bool _has_damage{false};
    float _damage{0.0F};  // Changed from unsigned short to float to avoid casting
    bool _dead{false};
    float _frame{0.0F};   // Changed from std::size_t to float
    float _x{0.0F};
    float _y{0.0F};
    std::vector<ImpactMarker> _impact_markers{};
public:
};

