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
    Base& operator=(const Base&) = default;
    Base& operator=(Base&&) noexcept = default;

    void reset() noexcept;
    void update(std::vector<Bullet>& i_bullets, unsigned short framecount);
    void draw(raylib::DrawSession& ds, const Texture2D& sprite) const;
    [[nodiscard]] Rectangle get_hitbox() const noexcept;
    [[nodiscard]] bool is_dead() const noexcept { return _dead; }
    // Change return type to int to match _damage type and avoid casting
    [[nodiscard]] int get_damage() const noexcept { return static_cast<int>(_damage); }
    
    // Records a bullet impact at the specified position
    void record_impact(float bullet_x, float bullet_y, float damage_amount);

private:
    static constexpr int MAX_IMPACTS = 32; // Maximum number of impacts to track
    std::vector<BulletImpact> _impacts{};  // Store impact locations
    float _damage{0.0f}; // Changed from unsigned short to float to avoid casting
    bool _dead{false};
    float _frame{0.0f}; // Changed from std::size_t to float
    float _x{0.0f};
    float _y{0.0f};
};

