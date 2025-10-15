#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"

// Structure to store impact data
struct BulletImpact {
    float x;      // X position relative to base
    float y;      // Y position relative to base
    float damage; // Damage amount at this location
};

class Base {
public:    Base() = default;
    explicit Base(float x) noexcept;
    ~Base() = default;
    Base(const Base&) = default;
    Base(Base&&) noexcept = default;
    auto operator=(const Base &) -> Base &     = default;
    auto operator=(Base &&) noexcept -> Base & = default;

    void reset() noexcept;
    void update(std::vector<Bullet>& i_bullets, GameTypes::Count framecount);
    void draw(raylib::DrawSession& ds, const Texture2D& sprite) const;
    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    [[nodiscard]] auto is_dead() const noexcept -> bool { return _dead; }
    // Change return type to int to match _damage type and avoid casting
    [[nodiscard]] auto get_damage() const noexcept -> int { return static_cast<int>(_damage); }

    // Records a bullet impact at the specified position
    void record_impact(float bullet_x, float bullet_y, float damage_amount);

private:
    static constexpr int MAX_IMPACTS = 32; // Maximum number of impacts to track
    std::vector<BulletImpact> _impacts;         // Store impact locations
    float _damage{0.0F}; // Changed from unsigned short to float to avoid casting
    bool _dead{false};
    float _frame{0.0F}; // Changed from std::size_t to float
    float _x{0.0F};
    float _y{0.0F};
};

