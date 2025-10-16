#include <raylib.h>
#include <cmath>
#include <algorithm>
#include "Bullet.hpp"
#include "Base.hpp"

Base::Base(float x) noexcept : _x(x), _y(GlobalConstant::SCREEN_HEIGHT - 3.0f * GlobalConstant::BASE_SIZE) {
    // Texture is initialized in reset()
}

Base::~Base() {
    if (_texture.id > 0) {
        UnloadTexture(_texture);
    }
}

Base::Base(Base&& other) noexcept : 
    _texture(other._texture),
    _damage(other._damage),
    _dead(other._dead),
    _frame(other._frame),
    _x(other._x),
    _y(other._y)
{
    // Prevent double free by setting the moved-from texture ID to 0
    other._texture.id = 0;
}

Base& Base::operator=(Base&& other) noexcept {
    if (this != &other) {
        // Free current resources
        if (_texture.id > 0) {
            UnloadTexture(_texture);
        }
        
        // Move resources from other
        _texture = other._texture;
        _damage = other._damage;
        _dead = other._dead;
        _frame = other._frame;
        _x = other._x;
        _y = other._y;
        
        // Prevent double free
        other._texture.id = 0;
    }
    return *this;
}

void Base::reset(const Image& baseImage) noexcept {
    _dead = false;
    _damage = 0;
    _frame = 0;
    
    // Unload current texture if it exists
    if (_texture.id > 0) {
        UnloadTexture(_texture);
    }
    
    // Extract just the first frame from the spritesheet
    Rectangle sourceRec = { 0.0f, 0.0f, GlobalConstant::BASE_WIDTH, static_cast<float>(baseImage.height) };
    Image baseCopy = ImageCopy(baseImage);
    ImageCrop(&baseCopy, sourceRec);
    
    // Load texture from the cropped image
    _texture = LoadTextureFromImage(baseCopy);
    UnloadImage(baseCopy);
}

void Base::update(std::vector<Bullet>& i_bullets, GameTypes::Count framecount) {
    if (_dead) return;
    
    // Calculate total damage for possible death check
    if (_damage >= framecount) {
        _frame = static_cast<float>(framecount);
        _dead = true;
        return;
    }
    
    // Check for new bullet impacts
    for (Bullet& bullet : i_bullets) {
        if (!bullet.IsDead() && CheckCollisionRecs(get_hitbox(), bullet.get_hitbox())) {
            // Calculate exact impact point relative to base position
            float rel_x = bullet.get_x() - _x;
            float rel_y = bullet.get_y() - _y;
            
            // Apply the impact to the texture
            apply_impact(rel_x, rel_y, 1.0f);
            
            // Update total damage and mark bullet as dead
            _damage += 1.0f;
            bullet.IsDead(true);
            break;
        }
    }
}

void Base::apply_impact(float rel_x, float rel_y, float damage_amount) {
    // Ensure impact is within base boundary
    rel_x = std::max(0.0f, std::min(rel_x, GlobalConstant::BASE_WIDTH));
    rel_y = std::max(0.0f, std::min(rel_y, GlobalConstant::BASE_SIZE));
    
    // Calculate radius for the impact
    float radius = 2.0f + damage_amount * 0.5f;
    
    // Convert to texture coordinates
    int tex_x = static_cast<int>(rel_x);
    int tex_y = static_cast<int>(rel_y);
    
    // Create the impact on the texture
    create_impact_on_texture(tex_x, tex_y, radius);
    
    // Create smaller impact fragments
    const int num_fragments = 4;
    for (int i = 0; i < num_fragments; i++) {
        float angle = static_cast<float>(i) * (2.0f * PI / num_fragments);
        float distance = radius * 1.2f;
        int frag_x = static_cast<int>(rel_x + cos(angle) * distance);
        int frag_y = static_cast<int>(rel_y + sin(angle) * distance);
        
        // Smaller radius for fragments
        float frag_radius = radius * 0.3f;
        create_impact_on_texture(frag_x, frag_y, frag_radius);
    }
}

void Base::create_impact_on_texture(int x, int y, float radius) {
    // Get texture data to directly modify pixels
    Image img = ::LoadImageFromTexture(_texture);
    
    // Calculate the area to modify
    int startX = std::max(0, x - static_cast<int>(radius));
    int startY = std::max(0, y - static_cast<int>(radius));
    int endX = std::min(img.width - 1, x + static_cast<int>(radius));
    int endY = std::min(img.height - 1, y + static_cast<int>(radius));
    
    // Create a circular impact by setting pixels transparent
    for (int py = startY; py <= endY; py++) {
        for (int px = startX; px <= endX; px++) {
            // Calculate distance from center of impact
            float dx = static_cast<float>(px - x);
            float dy = static_cast<float>(py - y);
            float distance = sqrtf(dx*dx + dy*dy);
            
            // If within radius, make pixel transparent
            if (distance <= radius) {
                // Set pixel to fully transparent (keep RGB but set alpha to 0)
                Color* pixelData = static_cast<Color*>(img.data);
                pixelData[py * img.width + px].a = 0;
            }
        }
    }
    
    // Update texture with modified image
    UpdateTexture(_texture, img.data);
    UnloadImage(img);
}

void Base::draw(raylib::DrawSession& ds) const {
    // Draw the modified texture
    const Vector2 dest{ _x, _y };
    ds.DrawTexture(_texture, dest.x, dest.y, WHITE);
}

Rectangle Base::get_hitbox() const noexcept {
    // Create hitbox that matches the texture dimensions exactly
    // This ensures proper collision detection with the visual representation
    return Rectangle(_x, _y, GlobalConstant::BASE_WIDTH, GlobalConstant::BASE_SIZE);
}





