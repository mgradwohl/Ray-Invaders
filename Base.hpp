#pragma once

// Standard library headers
#include <vector>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Bullet.hpp"
#include "Global.hpp"
#include "RLDrawSession.hpp"
#include "RLTexture2D.hpp"
#include "RLWaveSound.hpp"

class Base
{
  public:
    Base() = default;
    explicit Base(float x) noexcept;
    ~Base();
    // Disable copying since we have a texture resource
    Base(const Base &) = delete;
    auto operator=(const Base &) = delete;
    // Non-movable due to non-movable WaveSound member
    Base(Base &&) noexcept = delete;
    auto operator=(Base &&) noexcept -> Base & = delete;

    void reset(const Image &baseImage) noexcept;
    // Update base state and process bullet impacts. Emits global hit decals via HitManager.
    void update(std::vector<Bullet> &i_bullets, GameTypes::Count framecount, class HitManager &hits);
    void draw(raylib::DrawSession &ds) const;
    // Split draw into two passes to allow batching of blend state in Bases::draw
    void draw_base(raylib::DrawSession &ds) const;
    void draw_damage(raylib::DrawSession &ds) const;
    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    [[nodiscard]] auto is_dead() const noexcept -> bool
    {
        return _dead;
    }
    // Change return type to int to match _damage type and avoid casting
    [[nodiscard]] auto get_damage() const noexcept -> int
    {
        return static_cast<int>(_damage);
    }
    [[nodiscard]] auto has_damage() const noexcept -> bool
    {
        return _has_damage && _damage_tex.id() > 0;
    }

    // Records a bullet impact at the specified position and modifies the texture (damage mask)
    void apply_impact(float rel_x, float rel_y, float damage_amount);

  private:
    // Transient impact visuals moved to HitManager; no local transient impact storage

    raylib::Texture2DFromImage _texture; // Each base has its own texture
    // CPU-side damage mask (1==intact white, 0==destroyed black) and GPU texture
    // This allows pixel-precise mutation and sampling for hit-testing.
    Image _damage_image{};
    raylib::Texture2DFromImage _damage_tex;
    // Alpha mask of the base sprite so we only apply damage to visible pixels
    Image _base_alpha{};
    // Mark that damage texture needs GPU update (to avoid frequent uploads)
    mutable bool _damage_gpu_dirty{false};
    // Per-pixel boolean mask (1 = opaque/visible, 0 = transparent) derived from the base sprite
    std::vector<unsigned char> _base_mask;
    bool _has_damage{false};
    float _damage{0.0F}; // Changed from unsigned short to float to avoid casting
    bool _dead{false};
    float _frame{0.0F}; // Changed from std::size_t to float
    float _x{0.0F};
    float _y{0.0F};
    // Legacy base-local impact markers removed
    raylib::WaveSound _basehitsound;

  public:
};
