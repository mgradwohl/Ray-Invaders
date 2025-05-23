#include <raylib.h>
#include "Bullet.hpp"
#include "Base.hpp"

Base::Base(float x) noexcept : _x(x), _y(F::SCREEN_HEIGHT - 4.0f - (F::BASE_SIZE * 3.0f)) {}

void Base::reset() noexcept {
    _dead = false;
    _damage = 0;
}

void Base::update(std::vector<Bullet>& i_enemy_bullets, unsigned short framecount) {
    if (_dead) return;
    // _damage is now float, no cast needed
    _frame = _damage;
    if (_frame >= framecount) {
        // We still need to convert framecount
        _frame = static_cast<float>(framecount);
        _dead = true;
    }    for (Bullet& enemy_bullet : i_enemy_bullets) {
        if (CheckCollisionRecs(get_hitbox(), enemy_bullet.get_hitbox())) {
            _damage += 1.0f; // Incrementing float value appropriately
            enemy_bullet.IsDead(true);
            break;
        }
    }
}

void Base::draw(raylib::DrawSession& ds, const Texture2D& sprite) const {
    const Vector2 dest{ _x, _y };
    const Rectangle source{ F::BASE_WIDTH * _frame, 0.0f, F::BASE_WIDTH, F::BASE_SIZE };
    ds.DrawTexture(sprite, source, dest, WHITE);
}

Rectangle Base::get_hitbox() const noexcept {
    // The sprite width is 30 pixels but the actual visible hitbox is 28 pixels
    constexpr float hitbox_width = 28.0f;
    constexpr float x_offset = (F::BASE_WIDTH - hitbox_width) * 0.5f; // Center the 28px hitbox within the 30px sprite
    return Rectangle(_x + x_offset, _y, hitbox_width, F::BASE_SIZE * 3.0f);
}