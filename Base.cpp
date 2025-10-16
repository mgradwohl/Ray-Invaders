#include <raylib.h>
#include <cmath>
#include <algorithm>
#include "Bullet.hpp"
#include "Base.hpp"

// Debug runtime toggles
#ifdef DEBUG
static bool g_dbg_disable_fade = false;
static bool g_dbg_visual_mode = false; // paint damage pixels red for visibility
#endif

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

    // Keep a copy of the base image alpha for debugging/inspection and possible future use
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
    // Debug: report initialization state
    fprintf(stderr, "[DEBUG] Base::reset idx=%d this=%p _has_damage=%d impacts=%zu damage=%.1f\n",
        _index, static_cast<const void*>(this), static_cast<int>(_has_damage), _impacts.size(), _damage);
    fflush(stderr);
}

void Base::update(std::vector<Bullet>& i_bullets, GameTypes::Count framecount) {
    // Debug: log entry and bullet count
    fprintf(stderr, "[DEBUG] Entering Base::update idx=%d this=%p at (_x=%.1f,_y=%.1f) bullets=%zu framecount=%u dead=%d damage=%.1f frame=%.1f\n",
        _index, static_cast<const void*>(this), _x, _y, i_bullets.size(), static_cast<unsigned>(framecount), _dead, _damage, _frame);
    fflush(stderr);
    if (_dead) return;

#ifdef DEBUG
    // Debug: allow spawning an impact at the mouse position for interactive testing
    // Only apply the impact to the base instance whose hitbox contains the mouse.
    if (IsKeyPressed(KEY_T)) {
        Vector2 mp = GetMousePosition();
        const Rectangle hb = get_hitbox();
        if (CheckCollisionPointRec(mp, hb)) {
            float relx = mp.x - _x;
            float rely = mp.y - _y;
            fprintf(stderr, "[DBG-KEY] T pressed this=%p mouse=(%.1f,%.1f) rel=(%.1f,%.1f) -- applying to this base\n",
                    static_cast<const void*>(this), mp.x, mp.y, relx, rely);
            fflush(stderr);
            apply_impact(relx, rely, 1.0f);
        } else {
            fprintf(stderr, "[DBG-KEY] T pressed this=%p mouse=(%.1f,%.1f) not over this base, skipping\n",
                    static_cast<const void*>(this), mp.x, mp.y);
            fflush(stderr);
        }
    }
#ifdef DEBUG
    // Toggle fade with F (useful to keep damage persistent while debugging)
    if (IsKeyPressed(KEY_F)) {
        g_dbg_disable_fade = !g_dbg_disable_fade;
        fprintf(stderr, "[DBG-TOG] Fade %s\n", g_dbg_disable_fade ? "OFF" : "ON");
        fflush(stderr);
    }
    // Toggle visual mode with V (paint damage pixels red)
    if (IsKeyPressed(KEY_V)) {
        g_dbg_visual_mode = !g_dbg_visual_mode;
        fprintf(stderr, "[DBG-TOG] Visual mode %s\n", g_dbg_visual_mode ? "ON" : "OFF");
        fflush(stderr);
    }
    // K = force several impacts at mouse over this base to accumulate damage quickly
    if (IsKeyPressed(KEY_K)) {
        Vector2 mp = GetMousePosition();
        const Rectangle hb = get_hitbox();
        if (CheckCollisionPointRec(mp, hb)) {
            float relx = mp.x - _x;
            float rely = mp.y - _y;
            fprintf(stderr, "[DBG-KEY] K pressed this=%p -- forcing multi-impact at rel=(%.1f,%.1f)\n",
                    static_cast<const void*>(this), relx, rely);
            fflush(stderr);
            for (int i = 0; i < 6; ++i) apply_impact(relx, rely, 1.0f);
        }
    }
#endif
#endif

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

        // Detailed collision logging: show hitbox, bullet center, relative impact
        const float bullet_center_x = bulletHB.x + 0.5f * bulletHB.width;
        const float bullet_center_y = bulletHB.y + 0.5f * bulletHB.height;
        fprintf(stderr, "[DEBUG] CheckCollisionRecs this=%p base=(x=%.1f,y=%.1f,w=%.1f,h=%.1f) bullet=(x=%.1f,y=%.1f,w=%.1f,h=%.1f) center=(%.1f,%.1f) collided=%d\n",
                static_cast<const void*>(this), baseHB.x, baseHB.y, baseHB.width, baseHB.height,
                bulletHB.x, bulletHB.y, bulletHB.width, bulletHB.height,
                bullet_center_x, bullet_center_y,
                collided);
        fflush(stderr);

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

            // Log computed intersection and relative coords
            fprintf(stderr, "[DEBUG] Intersection this=%p inter=(x1=%.1f,y1=%.1f,w=%.1f,h=%.1f) impact_world=(%.1f,%.1f) rel=(%.1f,%.1f)\n",
                    static_cast<const void*>(this), inter_x1, inter_y1, inter_w, inter_h, impact_world_x, impact_world_y, rel_x, rel_y);
            fflush(stderr);

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
                fprintf(stderr, "[DEBUG] Center-sample idx=%d this=%p rel=(%.1f,%.1f) base_visible=%d alpha=%u counts=%d\n",
                        _index, static_cast<const void*>(this), rel_x, rel_y, static_cast<int>(base_visible), static_cast<unsigned>(alpha), static_cast<int>(collision_counts));
                    fflush(stderr);
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
                // diagnostic counters
                int candidate_px = 0;
                int skipped_by_mask = 0;
                int changed_px = 0;
                // Use compact base mask for opacity tests
                unsigned char* base_mask_ptr = _base_mask.empty() ? nullptr : _base_mask.data();
                unsigned char* dmg_ptr = reinterpret_cast<unsigned char*>(_damage_image.data);

                // Print a quick sanity report about local coords and image sizes
                if (px >= 0 && py >= 0 && px < _damage_image.width && py < _damage_image.height) {
                    int sidx = py * _damage_image.width + px;
                    unsigned char center_alpha = reinterpret_cast<unsigned char*>(_damage_image.data)[sidx * 4 + 3];
                    int mask_val = base_mask_ptr ? base_mask_ptr[sidx] : -1;
            fprintf(stderr, "[DBG-IMPACT] idx=%d this=%p px=%d py=%d sidx=%d dmg_w=%d dmg_h=%d mask=%d center_alpha=%u pr=%d\n",
                _index, static_cast<const void*>(this), px, py, sidx, _damage_image.width, _damage_image.height, mask_val, static_cast<unsigned>(center_alpha), pr);
                    fflush(stderr);
                } else {
            fprintf(stderr, "[DBG-IMPACT] idx=%d this=%p px=%d py=%d OUT_OF_BOUNDS dmg_w=%d dmg_h=%d\n",
                _index, static_cast<const void*>(this), px, py, _damage_image.width, _damage_image.height);
                    fflush(stderr);
                }

                for (int y = std::max(0, py - pr); y <= std::min(_damage_image.height - 1, py + pr); ++y) {
                    for (int x = std::max(0, px - pr); x <= std::min(_damage_image.width - 1, px + pr); ++x) {
                        const int dx = x - px;
                        const int dy = y - py;
                        const float dist = std::sqrt(static_cast<float>(dx*dx + dy*dy));
                        if (dist <= static_cast<float>(pr)) {
                            ++candidate_px;
                            // radial falloff [0..1]
                            const float f = 1.0f - (dist / static_cast<float>(pr));
                            const int incr = static_cast<int>(std::ceil(base_alpha_incr * f));
                            int idx = y * _damage_image.width + x;
                            if (!base_mask_ptr || base_mask_ptr[idx] == 0) {
                                ++skipped_by_mask;
                                continue; // skip transparent pixels
                            }
                            unsigned char* pix = dmg_ptr + idx * 4;
                            int old_alpha = static_cast<int>(pix[3]); // check current alpha
                            // Increase alpha to create opaque damage areas that will create holes via multiplicative blending
                            int new_alpha = std::min(255, old_alpha + incr);
                            if (new_alpha != old_alpha) {
                                // Set RGB to black for multiplicative blending (black * base = transparent holes)
                                if (g_dbg_visual_mode) {
                                    pix[0] = 255; pix[1] = 0; pix[2] = 0; // red debug
                                    pix[3] = 255; // full alpha for debug visibility
                                } else {
                                    // For alpha-based transparency: white RGB with alpha = transparency
                                    // High alpha = more transparent (holes), low alpha = more opaque (intact)
                                    pix[0] = 255; pix[1] = 255; pix[2] = 255; // white RGB
                                    pix[3] = new_alpha; // high alpha = transparent holes
                                }
                                any_changed = true;
                                ++changed_px;
                                if (changed_px <= 6) {
                                    fprintf(stderr, "[DBG-PX] idx=%d this=%p (update) pixel=(%d,%d) old_alpha=%d new_alpha=%d decr=%d\n",
                                            _index, static_cast<const void*>(this), x, y, old_alpha, new_alpha, incr);
                                    fflush(stderr);
                                }
                            }
                        }
                    }
                }
                if (any_changed) {
                    _damage_gpu_dirty = true;
                    _has_damage = true;
            fprintf(stderr, "[DEBUG] Damage applied idx=%d this=%p changed_pixels=%d candidates=%d skipped_by_mask=%d\n",
                _index, static_cast<const void*>(this), changed_px, candidate_px, skipped_by_mask);
                } else {
            fprintf(stderr, "[DEBUG] Impact registered but no damage pixels changed (transparent area?) idx=%d this=%p candidates=%d skipped_by_mask=%d\n",
                _index, static_cast<const void*>(this), candidate_px, skipped_by_mask);
                }
            }

            // Update total damage and mark bullet as dead
            _damage += 1.0f;
            bullet.IsDead(true);
            break;
        }
    }

    // Age debug markers and remove expired ones
    for (auto it = _debug_markers.begin(); it != _debug_markers.end();) {
        it->ttl -= 1;
        if (it->ttl <= 0) it = _debug_markers.erase(it);
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
        fprintf(stderr, "[DBG-TEX] UpdateTexture id=%d size=(%d,%d) idx=%d this=%p\n",
                _damage_tex.id, _damage_image.width, _damage_image.height, _index, static_cast<const void*>(this));
        fflush(stderr);
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
        // diagnostic counters
        int candidate_px = 0;
        int skipped_by_mask = 0;
        int changed_px = 0;
        unsigned char* base_mask_ptr = _base_mask.empty() ? nullptr : _base_mask.data();
        unsigned char* dmg_ptr = reinterpret_cast<unsigned char*>(_damage_image.data);

                if (px >= 0 && py >= 0 && px < _damage_image.width && py < _damage_image.height) {
                    int sidx = py * _damage_image.width + px;
                    unsigned char center_brightness = reinterpret_cast<unsigned char*>(_damage_image.data)[sidx * 4 + 0]; // RGB brightness
                    int mask_val = base_mask_ptr ? base_mask_ptr[sidx] : -1;
        fprintf(stderr, "[DBG-APPLY] idx=%d this=%p px=%d py=%d sidx=%d dmg_w=%d dmg_h=%d mask=%d center_bright=%u pr=%d\n",
            _index, static_cast<const void*>(this), px, py, sidx, _damage_image.width, _damage_image.height, mask_val, static_cast<unsigned>(center_brightness), pr);
                    fflush(stderr);
                } else {
        fprintf(stderr, "[DBG-APPLY] idx=%d this=%p px=%d py=%d OUT_OF_BOUNDS dmg_w=%d dmg_h=%d\n",
                _index, static_cast<const void*>(this), px, py, _damage_image.width, _damage_image.height);
                    fflush(stderr);
                }        for (int y = std::max(0, py - pr); y <= std::min(_damage_image.height - 1, py + pr); ++y) {
            for (int x = std::max(0, px - pr); x <= std::min(_damage_image.width - 1, px + pr); ++x) {
                const int dx = x - px;
                const int dy = y - py;
                const float dist = std::sqrt(static_cast<float>(dx*dx + dy*dy));
                if (dist <= static_cast<float>(pr)) {
                    ++candidate_px;
                    const float f = 1.0f - (dist / static_cast<float>(pr));
                    const int incr = static_cast<int>(std::ceil(base_alpha_incr * f));
                    int idx = y * _damage_image.width + x;
                    if (!base_mask_ptr || base_mask_ptr[idx] == 0) { ++skipped_by_mask; continue; } // skip transparent background
                    unsigned char* pix = dmg_ptr + idx * 4;
                    int old_brightness = static_cast<int>(pix[0]); // RGB should be same for grayscale
                    int new_brightness = std::max(0, old_brightness - incr);
                    if (new_brightness != old_brightness) {
                        // Set RGB to the new brightness level (for multiplicative blending)
                        if (g_dbg_visual_mode) {
                            pix[0] = 255; pix[1] = 0; pix[2] = 0; // red debug
                        } else {
                            pix[0] = new_brightness; pix[1] = new_brightness; pix[2] = new_brightness;
                        }
                        pix[3] = 255; // keep alpha at 255 for multiplicative blending
                        any_changed = true;
                        ++changed_px;
                        if (changed_px <= 6) {
                            fprintf(stderr, "[DBG-PX] idx=%d this=%p (apply) pixel=(%d,%d) old_bright=%d new_bright=%d decr=%d\n",
                                    _index, static_cast<const void*>(this), x, y, old_brightness, new_brightness, incr);
                            fflush(stderr);
                        }
                    }
                }
            }
        }
    if (any_changed) {
        _damage_gpu_dirty = true;
        _has_damage = true;
        fprintf(stderr, "[DBG-APPLY] idx=%d this=%p changed_pixels=%d candidates=%d skipped_by_mask=%d\n",
            _index, static_cast<const void*>(this), changed_px, candidate_px, skipped_by_mask);
    } else {
        fprintf(stderr, "[DBG-APPLY] idx=%d this=%p no pixels changed candidates=%d skipped_by_mask=%d\n",
            _index, static_cast<const void*>(this), candidate_px, skipped_by_mask);
    }
    }

    // Also add a debug marker (longer-lived for visibility)
    _debug_markers.push_back({ rel_x + 0.5f, rel_y + 0.5f, 30 });
}

// create_impact_on_texture removed - impacts are rendered at draw time using _impacts

void Base::draw(raylib::DrawSession& ds) const {
    // Debug: report draw-time state (helps track unexpected initial markers)
    fprintf(stderr, "[DEBUG] Base::draw idx=%d this=%p _has_damage=%d impacts=%zu damage=%.1f texture_id=%d\n",
        _index, static_cast<const void*>(this), static_cast<int>(_has_damage), _impacts.size(), _damage, _texture.id);
    fflush(stderr);

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
        
        fprintf(stderr, "[DBG-DRAW] idx=%d this=%p drew damage texture id=%d\n", _index, static_cast<const void*>(this), _damage_tex.id);
        fflush(stderr);
    } else {
        // No damage - just draw base normally
        const Vector2 dest{ _x, _y };
        ds.DrawTexture(_texture, dest.x, dest.y, WHITE);
        fprintf(stderr, "[DBG-DRAW] idx=%d this=%p SKIPPED damage texture _has_damage=%d damage_tex_id=%d\n", 
                _index, static_cast<const void*>(this), static_cast<int>(_has_damage), _damage_tex.id);
        fflush(stderr);
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

    // Draw debug markers
    for (const DebugMarker& m : _debug_markers) {
        ds.DrawCircle(_x + m.x, _y + m.y, 2.5f, RED);
    }

    // Low-frequency debug: sample damage image brightness at base center (or last impact) every 30 draw calls
    static int dbg_draw_tick = 0;
    dbg_draw_tick = (dbg_draw_tick + 1) % 30;
    if (dbg_draw_tick == 0 && _damage_image.data) {
        int cx = _damage_image.width / 2;
        int cy = _damage_image.height / 2;
        // if we have an impact, sample its location instead of pure center
        if (!_impacts.empty()) {
            const Impact& last = _impacts.back();
            int tx = static_cast<int>(std::floor(last.x));
            int ty = static_cast<int>(std::floor(last.y));
            if (tx >= 0 && tx < _damage_image.width && ty >= 0 && ty < _damage_image.height) {
                cx = tx; cy = ty;
            }
        }
        int sidx = cy * _damage_image.width + cx;
        unsigned char* pixel = reinterpret_cast<unsigned char*>(_damage_image.data) + sidx * 4;
        unsigned char alpha = pixel[3];
        fprintf(stderr, "[DBG-SAMPLE] idx=%d this=%p sample=(%d,%d) alpha=%u (RGBA=%u,%u,%u,%u)\n",
                _index, static_cast<const void*>(this), cx, cy, static_cast<unsigned>(alpha),
                static_cast<unsigned>(pixel[0]), static_cast<unsigned>(pixel[1]), static_cast<unsigned>(pixel[2]), static_cast<unsigned>(pixel[3]));
        fflush(stderr);
    }
}

Rectangle Base::get_hitbox() const noexcept {
    // Create hitbox that matches the texture dimensions exactly
    // This ensures proper collision detection with the visual representation
    // Use the actual texture height so the hitbox covers the visible area
    const float height = (_texture.id > 0) ? static_cast<float>(_texture.height) : GlobalConstant::BASE_SIZE;
    return Rectangle(_x, _y, GlobalConstant::BASE_WIDTH, height);
}





