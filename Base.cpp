// Corresponding header
#include "Base.hpp"

// Standard library headers
#include <algorithm>
#include <cmath>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Bullet.hpp"



Base::Base(float x) noexcept : _x(x), _y(GlobalConstant::SCREEN_HEIGHT - 3.0f * GlobalConstant::BASE_SIZE) {
    // Texture is initialized in reset()
}

Base::~Base() {
    if (_texture.id > 0) UnloadTexture(_texture);
    if (_damage_tex.id > 0) UnloadTexture(_damage_tex);
    if (_damage_image.data) UnloadImage(_damage_image);
    if (_base_alpha.data) UnloadImage(_base_alpha);
}

Base::Base(Base&& other) noexcept : 
    _texture(other._texture),
    _damage_image(other._damage_image),
    _damage_tex(other._damage_tex),
    _base_alpha(other._base_alpha),
    _damage(other._damage),
    _dead(other._dead),
    _frame(other._frame),
    _x(other._x),
    _y(other._y)
{
    // Prevent double free by setting the moved-from texture ID to 0
    other._texture.id = 0;
    other._damage_tex.id = 0;
    other._damage_image.data = nullptr;
    other._base_alpha.data = nullptr;
    other._base_mask.clear();
}

Base& Base::operator=(Base&& other) noexcept {
    if (this != &other) {
        // Free current resources
        if (_texture.id > 0) {
            UnloadTexture(_texture);
        }
        if (_damage_tex.id > 0) UnloadTexture(_damage_tex);
        if (_damage_image.data) UnloadImage(_damage_image);
        
        // Move resources from other
        _texture = other._texture;
        _damage_image = other._damage_image;
        _damage_tex = other._damage_tex;
    _base_alpha = other._base_alpha;
    _base_mask = std::move(other._base_mask);
        _damage = other._damage;
        _dead = other._dead;
        _frame = other._frame;
        _x = other._x;
        _y = other._y;
        
        // Prevent double free
        other._texture.id = 0;
        other._damage_tex.id = 0;
        other._damage_image.data = nullptr;
    other._base_alpha.data = nullptr;
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
    
    // Build a compact per-pixel mask (1 == visible, 0 == transparent) from the cropped baseCopy
    // before we release it. This ensures the mask maps exactly to the sprite pixels.
    const int bcw = baseCopy.width;
    const int bch = baseCopy.height;
    _base_mask.assign(bcw * bch, 0);
    unsigned char* bcptr = reinterpret_cast<unsigned char*>(baseCopy.data);
    for (int y = 0; y < bch; ++y) {
        for (int x = 0; x < bcw; ++x) {
            int idx = (y * bcw + x) * 4;
            unsigned char a = bcptr[idx + 3];
            _base_mask[y * bcw + x] = (a > 0) ? 1 : 0;
        }
    }

    // Keep a copy of the base image alpha for possible future use
    if (_base_alpha.data) UnloadImage(_base_alpha);
    _base_alpha = ImageCopy(baseCopy);

    // Load texture from the cropped image and free the copy
    _texture = LoadTextureFromImage(baseCopy);
    UnloadImage(baseCopy);
    
    // Initialize CPU damage image (transparent black == intact). Size matches base texture.
    if (_damage_image.data) {
        UnloadImage(_damage_image);
    }
    // Create damage image: start fully transparent (alpha=0) so it doesn't affect the base initially
    // Damaged pixels will become opaque (alpha=255) to act as holes in multiplicative blending
    _damage_image = GenImageColor(GlobalConstant::BASE_WIDTH, _texture.height, Color{255,255,255,0});
    // Create GPU texture for the damage mask
    if (_damage_tex.id > 0) UnloadTexture(_damage_tex);
    _damage_tex = LoadTextureFromImage(_damage_image);
    // No damage yet
    _impacts.clear();
    _has_damage = false;
}

void Base::update(std::vector<Bullet>& i_bullets, GameTypes::Count framecount) {
    if (_dead) return;



// Removed automatic one-time T-test; interactive T-key remains and only applies when mouse is over a base
    
    // Calculate total damage for possible death check
    // Only mark as dead when there are animation frames to progress through.
    // If framecount is zero (single-frame sprite) the base should not be
    // considered dead simply because _damage == 0.
    if (framecount > 0 && _damage >= framecount) {
        _frame = static_cast<float>(framecount);
        _dead = true;
        return;
    }
    
    // Check for new bullet impacts
    for (Bullet& bullet : i_bullets) {
        const Rectangle baseHB = get_hitbox();
        const Rectangle bulletHB = bullet.get_hitbox();
        bool collided = CheckCollisionRecs(baseHB, bulletHB);

        if (!bullet.IsDead() && collided) {
            // Use intersection rectangle center as impact point (more robust than bullet center)
            const float inter_x1 = std::max(baseHB.x, bulletHB.x);
            const float inter_y1 = std::max(baseHB.y, bulletHB.y);
            const float inter_x2 = std::min(baseHB.x + baseHB.width, bulletHB.x + bulletHB.width);
            const float inter_y2 = std::min(baseHB.y + baseHB.height, bulletHB.y + bulletHB.height);
            const float inter_w = std::max(0.0f, inter_x2 - inter_x1);
            const float inter_h = std::max(0.0f, inter_y2 - inter_y1);
            const float impact_world_x = inter_x1 + 0.5f * inter_w;
            const float impact_world_y = inter_y1 + 0.5f * inter_h;

            // Convert to base-local coordinates
            const float rel_x = impact_world_x - _x;
            const float rel_y = impact_world_y - _y;

            // Record an impact (do not mutate texture immediately for visuals)
            const float radius = 2.0f + 0.5f * 1.0f; // damage_amount=1.0f

            // Use the center pixel alpha as the collision test. A pixel alpha >= threshold
            // is treated as destroyed; bullets pass through destroyed pixels.
            bool collision_counts = true;
            if (_damage_image.data) {
                const int cx = static_cast<int>(std::floor(rel_x));
                const int cy = static_cast<int>(std::floor(rel_y));
                if (cx >= 0 && cx < _damage_image.width && cy >= 0 && cy < _damage_image.height) {
                    int sidx = cy * _damage_image.width + cx;
                    unsigned char* spix = reinterpret_cast<unsigned char*>(_damage_image.data) + sidx * 4;
                    const unsigned char alpha = spix[3]; // check alpha transparency, lower = more damaged
                    // Also ensure the base sprite at this pixel is visible; if not, the bullet hit empty space
                    bool base_visible = (_base_mask.empty() ? true : (_base_mask[sidx] != 0));
                    if (!base_visible) collision_counts = false;
                    // If alpha is high (opaque damage), pixel is damaged and bullets pass through
                    const unsigned char alpha_threshold = 128; // above this = damaged/destroyed
                    if (alpha >= alpha_threshold) collision_counts = false;

                }
            }

            if (!collision_counts) continue;

            _impacts.push_back({ rel_x, rel_y, radius, 60 }); // visible for 60 frames

            // Mutate the CPU damage image: increase alpha in a radial falloff so damage accumulates
            if (_damage_image.data) {
                // Prepare per-hit alpha increment (higher -> faster destruction)
                const int base_alpha_incr = static_cast<int>(std::ceil(128.0f * 2.5f)); // increased damage per hit - was 1.0f
                const int px = static_cast<int>(std::floor(rel_x));
                const int py = static_cast<int>(std::floor(rel_y));
                const int pr = static_cast<int>(std::ceil(radius)) + 1;
                bool any_changed = false;

                // Use compact base mask for opacity tests
                unsigned char* base_mask_ptr = _base_mask.empty() ? nullptr : _base_mask.data();
                unsigned char* dmg_ptr = reinterpret_cast<unsigned char*>(_damage_image.data);

                // Print a quick sanity report about local coords and image sizes
                // Bounds check for the impact center point
                if (px >= 0 && py >= 0 && px < _damage_image.width && py < _damage_image.height) {
                    // Impact point is within bounds, proceed with damage application
                } else {
                    // Impact point is outside bounds, skip damage application
                }

                for (int y = std::max(0, py - pr); y <= std::min(_damage_image.height - 1, py + pr); ++y) {
                    for (int x = std::max(0, px - pr); x <= std::min(_damage_image.width - 1, px + pr); ++x) {
                        const int dx = x - px;
                        const int dy = y - py;
                        const float dist = std::sqrt(static_cast<float>(dx*dx + dy*dy));
                        if (dist <= static_cast<float>(pr)) {
                            // radial falloff [0..1]
                            const float f = 1.0f - (dist / static_cast<float>(pr));
                            const int incr = static_cast<int>(std::ceil(base_alpha_incr * f));
                            int idx = y * _damage_image.width + x;
                            if (!base_mask_ptr || base_mask_ptr[idx] == 0) {
                                continue; // skip transparent pixels
                            }
                            unsigned char* pix = dmg_ptr + idx * 4;
                            int old_alpha = static_cast<int>(pix[3]); // check current alpha
                            // Increase alpha to create opaque damage areas that will create holes via multiplicative blending
                            int new_alpha = std::min(255, old_alpha + incr);
                            if (new_alpha != old_alpha) {
                                // Set RGB to black for multiplicative blending (black * base = transparent holes)
                                // For alpha-based transparency: white RGB with alpha = transparency
                                // High alpha = more transparent (holes), low alpha = more opaque (intact)
                                pix[0] = 255; pix[1] = 255; pix[2] = 255; // white RGB
                                pix[3] = new_alpha; // high alpha = transparent holes
                                any_changed = true;
                            }
                        }
                    }
                }
                if (any_changed) {
                    _damage_gpu_dirty = true;
                    _has_damage = true;
                }
            }

            // Update total damage and mark bullet as dead
            _damage += 1.0f;
            bullet.IsDead(true);
            break;
        }
    }

    // Age impact markers and remove expired ones
    for (auto it = _impact_markers.begin(); it != _impact_markers.end();) {
        it->ttl -= 1;
        if (it->ttl <= 0) it = _impact_markers.erase(it);
        else ++it;
    }

    // No fade logic - damage is now permanent (transparent holes stay transparent)

        // Age and fade impacts: keep permanent impacts with ttl==0, otherwise decrement and erase
        for (auto it = _impacts.begin(); it != _impacts.end();) {
            if (it->ttl > 0) {
                it->ttl -= 1;
                if (it->ttl <= 0) it = _impacts.erase(it);
                else ++it;
            } else {
                // ttl == 0 means permanent impact
                ++it;
            }
        }

    // If any damage pixels were changed this frame (either new impacts or fade), update GPU texture
    if (_damage_gpu_dirty && _damage_image.data) {
        UpdateTexture(_damage_tex, _damage_image.data);
        _damage_gpu_dirty = false;
    }
}

void Base::apply_impact(float rel_x, float rel_y, float damage_amount) {
    // Record impact into the impact list for draw-time rendering
    // Clamp rel coords to the damage image / texture dimensions (avoid clamping to generic constants)
    if (_damage_image.data) {
        const float maxx = static_cast<float>(std::max(1, _damage_image.width) - 1);
        const float maxy = static_cast<float>(std::max(1, _damage_image.height) - 1);
        rel_x = std::max(0.0f, std::min(rel_x, maxx));
        rel_y = std::max(0.0f, std::min(rel_y, maxy));
    } else {
        rel_x = std::max(0.0f, std::min(rel_x, GlobalConstant::BASE_WIDTH));
        rel_y = std::max(0.0f, std::min(rel_y, GlobalConstant::BASE_SIZE));
    }
    float radius = 2.0f + damage_amount * 0.5f;
    _impacts.push_back({ rel_x, rel_y, radius, 60 });

    // Mutate the CPU damage image similarly to update()
    if (_damage_image.data) {
        // increase alpha like in update(): radial falloff per-impact
        const int base_alpha_incr = static_cast<int>(std::ceil(128.0f * damage_amount * 2.5f)); // increased damage per hit
        const int px = static_cast<int>(std::floor(rel_x));
        const int py = static_cast<int>(std::floor(rel_y));
        const int pr = static_cast<int>(std::ceil(radius)) + 1;
        bool any_changed = false;

        unsigned char* base_mask_ptr = _base_mask.empty() ? nullptr : _base_mask.data();
        unsigned char* dmg_ptr = reinterpret_cast<unsigned char*>(_damage_image.data);


        
        for (int y = std::max(0, py - pr); y <= std::min(_damage_image.height - 1, py + pr); ++y) {
            for (int x = std::max(0, px - pr); x <= std::min(_damage_image.width - 1, px + pr); ++x) {
                const int dx = x - px;
                const int dy = y - py;
                const float dist = std::sqrt(static_cast<float>(dx*dx + dy*dy));
                if (dist <= static_cast<float>(pr)) {
                    const float f = 1.0f - (dist / static_cast<float>(pr));
                    const int incr = static_cast<int>(std::ceil(base_alpha_incr * f));
                    int idx = y * _damage_image.width + x;
                    if (!base_mask_ptr || base_mask_ptr[idx] == 0) { continue; } // skip transparent background
                    unsigned char* pix = dmg_ptr + idx * 4;
                    int old_brightness = static_cast<int>(pix[0]); // RGB should be same for grayscale
                    int new_brightness = std::max(0, old_brightness - incr);
                    if (new_brightness != old_brightness) {
                        // Set RGB to the new brightness level (for multiplicative blending)
                        pix[0] = new_brightness; pix[1] = new_brightness; pix[2] = new_brightness;
                        pix[3] = 255; // keep alpha at 255 for multiplicative blending
                        any_changed = true;
                    }
                }
            }
        }
        if (any_changed) {
            _damage_gpu_dirty = true;
            _has_damage = true;
        }
    }

    // Also add an impact marker (longer-lived for visibility)
    _impact_markers.push_back({ rel_x + 0.5f, rel_y + 0.5f, 30 });
}

// create_impact_on_texture removed - impacts are rendered at draw time using _impacts

void Base::draw(raylib::DrawSession& ds) const {
    // Apply damage by masking the base texture with the damage alpha channel
    if (_has_damage && _damage_tex.id > 0) {
        // Use a simple approach: tint the base texture using the damage texture as an alpha mask
        // We'll draw the base texture with a custom shader/approach that uses damage alpha
        
        // For now, let's try drawing with normal blending and see the effect
        const Rectangle src = { 0.0f, 0.0f, static_cast<float>(_texture.width), static_cast<float>(_texture.height) };
        const Vector2 pos = { _x, _y };
        
        // Draw the base texture normally first
        ds.DrawTexture(_texture, src, pos, WHITE);
        
        // Now overlay damage by using alpha blending with inverted colors
        // High alpha damage areas = more transparent in final result
        BeginBlendMode(BLEND_ALPHA);
        const Rectangle damage_src = { 0.0f, 0.0f, static_cast<float>(_damage_tex.width), static_cast<float>(_damage_tex.height) };
        
        // Draw damage texture with a color that creates holes - transparent black
        Color hole_color = {0, 0, 0, 128}; // Semi-transparent black to create visible holes
        ds.DrawTexture(_damage_tex, damage_src, pos, hole_color);
        EndBlendMode();
    } else {
        // No damage - just draw base normally
        const Vector2 dest{ _x, _y };
        ds.DrawTexture(_texture, dest.x, dest.y, WHITE);
    }

    // Draw transient impacts recorded (draw-time rendering) as red flashes
    for (const Impact& imp : _impacts) {
        unsigned char alpha = 255;
        if (imp.ttl > 0) {
            const int maxTtl = 60;
            const int ttl = std::min(imp.ttl, maxTtl);
            alpha = static_cast<unsigned char>(50 + (205 * ttl) / maxTtl);
        }
        Color holeColor = Color{ 200, 40, 40, alpha };
        Color outline = Color{ 255, 120, 120, static_cast<unsigned char>(alpha) };
        ds.DrawCircle(_x + imp.x, _y + imp.y, imp.radius, holeColor);
        ds.DrawCircle(_x + imp.x, _y + imp.y, imp.radius + 1.0f, outline);
    }

    // Draw impact markers
    for (const ImpactMarker& m : _impact_markers) {
        ds.DrawCircle(_x + m.x, _y + m.y, 2.5f, RED);
    }
}

Rectangle Base::get_hitbox() const noexcept {
    // Create hitbox that matches the texture dimensions exactly
    // This ensures proper collision detection with the visual representation
    // Use the actual texture height so the hitbox covers the visible area
    const float height = (_texture.id > 0) ? static_cast<float>(_texture.height) : GlobalConstant::BASE_SIZE;
    return Rectangle(_x, _y, GlobalConstant::BASE_WIDTH, height);
}





