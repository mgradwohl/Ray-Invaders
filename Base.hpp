#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"

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

private:
    // Creates a circular impact on the texture at the specified position
    void create_impact_on_texture(int x, int y, float radius);
    
    Texture2D _texture{}; // Each base has its own texture
    float _damage{0.0F};  // Changed from unsigned short to float to avoid casting
    bool _dead{false};
    float _frame{0.0F};   // Changed from std::size_t to float
    float _x{0.0F};
    float _y{0.0F};
};

