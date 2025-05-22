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
    explicit Base(unsigned short x) noexcept;
    ~Base() = default;
    Base(const Base&) = default;
    Base(Base&&) noexcept = default;
    Base& operator=(const Base&) = default;
    Base& operator=(Base&&) noexcept = default;

    void reset() noexcept;
    void update(std::vector<Bullet>& i_enemy_bullets, unsigned short framecount);
    void draw(raylib::DrawSession& ds, const Texture2D& sprite) const;

    [[nodiscard]] Rectangle get_hitbox() const noexcept;
    [[nodiscard]] bool is_dead() const noexcept { return _dead; }
    [[nodiscard]] unsigned short get_damage() const noexcept { return _damage; }

private:
    unsigned short _damage = 0;
    bool _dead = false;
    std::size_t _frame = 0;
    float _x = 0;
    float _y = 0;
};

