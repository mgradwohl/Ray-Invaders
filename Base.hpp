#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"

class Base {
public:    Base() = default;
    explicit Base(float x) noexcept;
    ~Base() = default;
    Base(const Base&) = default;
    Base(Base&&) noexcept = default;
    Base& operator=(const Base&) = default;
    Base& operator=(Base&&) noexcept = default;

    void reset() noexcept;
    void update(std::vector<Bullet>& i_enemy_bullets, unsigned short framecount);    void draw(raylib::DrawSession& ds, const Texture2D& sprite) const;    [[nodiscard]] Rectangle get_hitbox() const noexcept;
    [[nodiscard]] bool is_dead() const noexcept { return _dead; }
    // Change return type to int to match _damage type and avoid casting
    [[nodiscard]] int get_damage() const noexcept { return static_cast<int>(_damage); }

private:
    float _damage{0.0f}; // Changed from unsigned short to float to avoid casting
    bool _dead{false};
    float _frame{0.0f}; // Changed from std::size_t to float
    float _x{0.0f};
    float _y{0.0f};
};

