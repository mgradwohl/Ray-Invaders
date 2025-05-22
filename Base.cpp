#include <raylib.h>
#include "Bullet.hpp"
#include "Base.hpp"

Base::Base(unsigned short x) noexcept : _x(static_cast<float>(x)), _y(static_cast<float>(SCREEN_HEIGHT - 4 - (BASE_SIZE * 3))) {}

void Base::reset() noexcept {
    _dead = false;
    _damage = 0;
}

void Base::update(std::vector<Bullet>& i_enemy_bullets, unsigned short framecount) {
    if (_dead) return;
    _frame = static_cast<std::size_t>(_damage);
    if (_frame >= static_cast<std::size_t>(framecount)) {
        _frame = static_cast<std::size_t>(framecount);
        _dead = true;
    }
    for (Bullet& enemy_bullet : i_enemy_bullets) {
        if (CheckCollisionRecs(get_hitbox(), enemy_bullet.get_hitbox())) {
            _damage++;
            enemy_bullet.IsDead(true);
            break;
        }
    }
}

void Base::draw(raylib::DrawSession& ds, const Texture2D& sprite) const {
    const Vector2 dest{ static_cast<float>(_x), static_cast<float>(_y) };
    const Rectangle source{ static_cast<float>(BASE_WIDTH * _frame), 0.0f, static_cast<float>(BASE_WIDTH), static_cast<float>(BASE_SIZE) };
    ds.DrawTexture(sprite, source, dest, WHITE);
}

Rectangle Base::get_hitbox() const noexcept {
    // Set hitbox to match the actual visible sprite width from analysis (93.3% for most frames)
    constexpr float hitbox_scale = 0.933f; // Based on sprite bitmap analysis
    constexpr float x_offset = (1.0f - hitbox_scale) * 0.5f * BASE_WIDTH; // Center the hitbox
    return Rectangle(_x + x_offset, _y, BASE_WIDTH * hitbox_scale, BASE_SIZE * 3);
}