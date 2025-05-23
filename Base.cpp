#include <raylib.h>
#include <cmath>
#include <algorithm>
#include "Bullet.hpp"
#include "Base.hpp"

Base::Base(float x) noexcept : _x(x), _y(F::SCREEN_HEIGHT - 3.0f * F::BASE_SIZE) {}

void Base::reset() noexcept {
    _dead = false;
    _damage = 0;
    _impacts.clear();
}

void Base::update(std::vector<Bullet>& i_bullets, unsigned short framecount) {
    if (_dead) return;
    
    // Calculate total damage from all impacts
    _frame = _damage;
    if (_frame >= framecount) {
        // We still need to convert framecount
        _frame = static_cast<float>(framecount);
        _dead = true;
    }
    
    // Check for new bullet impacts
    for (Bullet& bullet : i_bullets) {
        if (!bullet.IsDead() && CheckCollisionRecs(get_hitbox(), bullet.get_hitbox())) {
            // Calculate exact impact point relative to base position
            float rel_x = bullet.get_x() - _x;
            float rel_y = bullet.get_y() - _y;
            
            // Record the impact
            record_impact(rel_x, rel_y, 1.0f);
            
            // Update total damage and mark bullet as dead
            _damage += 1.0f;
            bullet.IsDead(true);
            break;
        }
    }
}

void Base::record_impact(float bullet_x, float bullet_y, float damage_amount) {
    // Ensure bullet impact is within base boundary
    bullet_x = std::max(0.0f, std::min(bullet_x, F::BASE_WIDTH));
    bullet_y = std::max(0.0f, std::min(bullet_y, F::BASE_SIZE));
    
    // Add to impacts collection
    if (_impacts.size() < MAX_IMPACTS) {
        _impacts.push_back({bullet_x, bullet_y, damage_amount});
    }
}

void Base::draw(raylib::DrawSession& ds, const Texture2D& sprite) const {
    // Draw base sprite using current frame (intact or basic damage)
    const Vector2 dest{ _x, _y };
    const Rectangle source{ F::BASE_WIDTH * _frame, 0.0f, F::BASE_WIDTH, F::BASE_SIZE };
    ds.DrawTexture(sprite, source, dest, WHITE);
      // Draw impact damage at each recorded hit location
    for (const auto& impact : _impacts) {
        // Create a realistic bullet impact pattern
        // First, draw the main impact hole
        float radius = 2.0f + impact.damage * 0.5f;
        ds.DrawCircle(
            _x + impact.x,
            _y + impact.y,
            radius,
            BLACK  // Black color for the impact hole
        );
        
        // Add some random smaller damage fragments around the main impact
        const int num_fragments = 4;  // Number of fragments per impact
        for (int i = 0; i < num_fragments; i++) {
            // Use trig to position fragments in a circular pattern around impact
            float angle = static_cast<float>(i) * (2.0f * PI / num_fragments);
            float distance = radius * 1.2f;
            float frag_x = _x + impact.x + cos(angle) * distance;
            float frag_y = _y + impact.y + sin(angle) * distance;
            
            // Smaller radius for fragments
            float frag_radius = radius * 0.3f;
            ds.DrawCircle(frag_x, frag_y, frag_radius, BLACK);
        }
    }
}

Rectangle Base::get_hitbox() const noexcept {
    // The sprite width is 30 pixels but the actual visible hitbox is 28 pixels
    constexpr float hitbox_width = 28.0f;
    constexpr float x_offset = (F::BASE_WIDTH - hitbox_width) * 0.5f; // Center the 28px hitbox within the 30px sprite
    return Rectangle(_x + x_offset, _y, hitbox_width, F::BASE_SIZE);
}