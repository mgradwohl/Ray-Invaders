// Corresponding header
#include "Base.hpp"

// Standard library headers
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <span>
#include <cstddef>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Bullet.hpp"
#include "Collision.hpp"
#include "Global.hpp"
#include "HitManager.hpp"
#include "RLWaveSound.hpp"

Base::Base(float x) noexcept
    : _x(x), _y(GlobalConstant::SCREEN_HEIGHT - (3.0F * GlobalConstant::BASE_SIZE)), _basehitsound("Resources/Sounds/Base Hit.wav")
{
    // Texture is initialized in reset()
}

namespace
{
using Pixel = std::array<std::uint8_t, 4>; // RGBA8

inline auto pixel_view(Image &img) -> std::span<Pixel>
{
    assert(img.data != nullptr);
    assert(img.width > 0 && img.height > 0);
    // This code assumes uncompressed RGBA8 layout (4 bytes per pixel)
    assert(img.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    const std::size_t count = static_cast<std::size_t>(img.width) * static_cast<std::size_t>(img.height);
    return {static_cast<Pixel *>(img.data), count};
}

// inline auto pixel_view(const Image &img) -> std::span<const Pixel>
// {
//     assert(img.data != nullptr);
//     assert(img.width > 0 && img.height > 0);
//     assert(img.format == PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
//     const std::size_t count = static_cast<std::size_t>(img.width) * static_cast<std::size_t>(img.height);
//     return {static_cast<const Pixel *>(img.data), count};
// }
} // namespace

Base::~Base()
{
    // RAII wrappers handle Texture unloading; explicitly free Image data if present
    if (_damage_image.data != nullptr)
    {
        UnloadImage(_damage_image);
    }
    if (_base_alpha.data != nullptr)
    {
        UnloadImage(_base_alpha);
    }
}

void Base::reset(const Image &baseImage) noexcept
{
    _dead = false;
    _damage = 0;
    _frame = 0;

    // Unload current texture if it exists (handled by RAII wrapper)
    _texture.unload();

    // Extract just the first frame from the spritesheet
    Rectangle sourceRec = {0.0F, 0.0F, GlobalConstant::BASE_WIDTH, static_cast<float>(baseImage.height)};
    Image baseCopy = ImageCopy(baseImage);
    ImageCrop(&baseCopy, sourceRec);
    // Ensure a known pixel layout for typed access
    ImageFormat(&baseCopy, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    // Build a compact per-pixel mask (1 == visible, 0 == transparent) from the cropped baseCopy
    // before we release it. This ensures the mask maps exactly to the sprite pixels.
    const int bcw = baseCopy.width;
    const int bch = baseCopy.height;
    _base_mask.assign(static_cast<std::size_t>(bcw) * static_cast<std::size_t>(bch), 0);
    auto basePixels = pixel_view(baseCopy);
    for (int y = 0; y < bch; ++y)
    {
        for (int x = 0; x < bcw; ++x)
        {
            const std::size_t sidx = (static_cast<std::size_t>(y) * static_cast<std::size_t>(bcw)) + static_cast<std::size_t>(x);
            const std::uint8_t a = basePixels[sidx][3];
            _base_mask[sidx] = (a > 0U) ? 1U : 0U;
        }
    }

    // Keep a copy of the base image alpha for possible future use
    if (_base_alpha.data != nullptr)
    {
        UnloadImage(_base_alpha);
    }
    _base_alpha = ImageCopy(baseCopy);

    // Load texture from the cropped image and free the copy
    _texture.load(baseCopy);
    UnloadImage(baseCopy);

    // Initialize CPU damage image (transparent black == intact). Size matches base texture.
    if (_damage_image.data != nullptr)
    {
        UnloadImage(_damage_image);
    }
    // Create damage image: start fully transparent (alpha=0) so it doesn't affect the base
    // initially Damaged pixels will become opaque (alpha=255) to act as holes in multiplicative
    // blending
    _damage_image = GenImageColor(GlobalConstant::BASE_WIDTH, _texture.height(), Color{255, 255, 255, 0});
    // Create GPU texture for the damage mask
    _damage_tex.load(_damage_image);
    // No damage yet
    _has_damage = false;
}

void Base::update(std::vector<Bullet> &i_bullets, GameTypes::Count framecount, HitManager &hits)
{
    if (_dead)
    {
        return;
    }

    // Calculate total damage for possible death check
    // Only mark as dead when there are animation frames to progress through.
    // If framecount is zero (single-frame sprite) the base should not be
    // considered dead simply because _damage == 0.
    if (framecount > 0 && _damage >= framecount)
    {
        _frame = static_cast<float>(framecount);
        _dead = true;
        return;
    }

    // Check for new bullet impacts
    const Rectangle baseHB = get_hitbox();
    const float base_x2 = baseHB.x + baseHB.width;
    const float base_y2 = baseHB.y + baseHB.height;
    for (Bullet &bullet : i_bullets)
    {
        const Rectangle bulletHB = bullet.get_hitbox();
        bool collided = AabbIntersect(baseHB, bulletHB);

        if (!bullet.IsDead() && collided)
        {
            // Use intersection rectangle center as impact point (more robust than bullet center)
            const float inter_x1 = std::max(baseHB.x, bulletHB.x);
            const float inter_y1 = std::max(baseHB.y, bulletHB.y);
            const float inter_x2 = std::min(base_x2, bulletHB.x + bulletHB.width);
            const float inter_y2 = std::min(base_y2, bulletHB.y + bulletHB.height);
            const float inter_w = std::max(0.0F, inter_x2 - inter_x1);
            const float inter_h = std::max(0.0F, inter_y2 - inter_y1);
            const float impact_world_x = inter_x1 + (GlobalConstant::HALF * inter_w);
            const float impact_world_y = inter_y1 + (GlobalConstant::HALF * inter_h);

            // Convert to base-local coordinates
            const float rel_x = impact_world_x - _x;
            const float rel_y = impact_world_y - _y;

            // Record an impact (do not mutate texture immediately for visuals)
            const float radius = 2.5F;// + 0.5F * 1.0F; // damage_amount=1.0F

            // Use the center pixel alpha as the collision test. A pixel alpha >= threshold
            // is treated as destroyed; bullets pass through destroyed pixels.
            bool collision_counts = true;
            if (_damage_image.data != nullptr)
            {
                const int cx = static_cast<int>(std::floor(rel_x));
                const int cy = static_cast<int>(std::floor(rel_y));
                if (cx >= 0 && cx < _damage_image.width && cy >= 0 && cy < _damage_image.height)
                {
                    // calculate where in the damage image the impact point maps to
                    int sidx = (cy * _damage_image.width) + cx;

                    // Read alpha at that location via a typed pixel view
                    auto pixels = pixel_view(_damage_image);
                    const std::uint8_t alpha = pixels[static_cast<std::size_t>(sidx)][3];
                    // Ensure the base sprite at this pixel is visible; if not, the bullet hit empty space
                    bool base_visible = (_base_mask.empty() ? true : (_base_mask[static_cast<std::size_t>(sidx)] != 0));
                    if (!base_visible)
                    {
                        collision_counts = false;
                    }
                    // If alpha is high (opaque damage), pixel is damaged and bullets pass through
                    const std::uint8_t alpha_threshold = 128; // above this = damaged/destroyed
                    if (alpha >= alpha_threshold)
                    {
                        collision_counts = false;
                    }
                }
            }

            if (!collision_counts)
            {
               continue;
            }

            // Emit a global hit decal in world space at the impact point and play the sound effect
            hits.add_hit(HitSubject::Base, HitOutcome::NonFatal, _x + rel_x, _y + rel_y, radius);
            [[maybe_unused]] bool played = _basehitsound.Play();

            // Mutate the CPU damage image: increase alpha in a radial falloff so damage accumulates
            if (_damage_image.data != nullptr)
            {
                // Prepare per-hit alpha increment (higher -> faster destruction)
                const int base_alpha_incr = static_cast<int>(std::ceil(128.0F * 2.5F));
                const int px = static_cast<int>(std::floor(rel_x));
                const int py = static_cast<int>(std::floor(rel_y));
                const int pr = static_cast<int>(std::ceil(radius)) + 1;
                const int pr_squared = pr * pr;
                const auto pr_float = static_cast<float>(pr);
                bool any_changed = false;

                // Typed views for compact base mask and pixel buffer
                std::span<const std::uint8_t> baseMask{_base_mask.data(), _base_mask.size()};
                auto pixelsWrite = pixel_view(_damage_image);

                for (int y = std::max(0, py - pr); y <= std::min(_damage_image.height - 1, py + pr); ++y)
                {
                    for (int x = std::max(0, px - pr); x <= std::min(_damage_image.width - 1, px + pr); ++x)
                    {
                        const int dx = x - px;
                        const int dy = y - py;
                        const int dist_squared = (dx * dx) + (dy * dy);
                        if (dist_squared <= pr_squared)
                        {
                            // Only calculate sqrt for pixels within radius (much fewer
                            // calculations)
                            const float dist = std::sqrt(static_cast<float>(dist_squared));
                            const float f = 1.0F - (dist / pr_float);
                            const int incr = static_cast<int>(std::ceil(base_alpha_incr * f));
                            int idx = (y * _damage_image.width) + x;
                            if (baseMask.empty() || baseMask[static_cast<std::size_t>(idx)] == 0)
                            {
                                continue; // skip transparent pixels
                            }
                            auto &pix = pixelsWrite[static_cast<std::size_t>(idx)];
                            int old_alpha = static_cast<int>(pix[3]); // current alpha
                            // Increase alpha to create opaque damage areas that will create holes
                            // via multiplicative blending
                            int new_alpha = std::min(255, old_alpha + incr);
                            if (new_alpha != old_alpha)
                            {
                                // Set RGB to black for multiplicative blending (black * base =
                                // transparent holes) For alpha-based transparency: white RGB with
                                // alpha = transparency High alpha = more transparent (holes), low
                                // alpha = more opaque (intact)
                                pix[0] = 255;
                                pix[1] = 255;
                                pix[2] = 255;       // white RGB
                                pix[3] = static_cast<std::uint8_t>(new_alpha); // high alpha = transparent holes
                                any_changed = true;
                            }
                        }
                    }
                }
                if (any_changed)
                {
                    _damage_gpu_dirty = true;
                    _has_damage = true;
                }
            }

            // Update total damage and mark bullet as dead
            _damage += 1.0F;
            bullet.IsDead(true);
            break;
    }

    // If any damage pixels were changed this frame (either new impacts or fade), update GPU texture
    if (_damage_gpu_dirty && (_damage_image.data != nullptr))
    {
        _damage_tex.update(_damage_image.data);
        _damage_gpu_dirty = false;
    }
    }
}

void Base::apply_impact(float rel_x, float rel_y, float damage_amount)
{
    // Clamp rel coords to the damage image / texture dimensions (avoid clamping to generic
    // constants) Clamp rel coords to the damage image / texture dimensions (avoid clamping to
    // generic constants)
    if (_damage_image.data != nullptr)
    {
        const float maxx = static_cast<float>(std::max(1, _damage_image.width) - 1);
        const float maxy = static_cast<float>(std::max(1, _damage_image.height) - 1);
        rel_x = std::max(0.0F, std::min(rel_x, maxx));
        rel_y = std::max(0.0F, std::min(rel_y, maxy));
    }
    else
    {
        rel_x = std::max(0.0F, std::min(rel_x, GlobalConstant::BASE_WIDTH));
        rel_y = std::max(0.0F, std::min(rel_y, GlobalConstant::BASE_SIZE));
    }
    // Mutate the CPU damage image similarly to update()
    if (_damage_image.data != nullptr)
    {
        // increase alpha like in update(): radial falloff per-impact
        const int base_alpha_incr = static_cast<int>(std::ceil(128.0F * damage_amount * 2.5F)); // increased damage per hit
        const int px = static_cast<int>(std::floor(rel_x));
        const int py = static_cast<int>(std::floor(rel_y));
        const int pr = static_cast<int>(std::ceil(2.0F + (damage_amount * GlobalConstant::HALF))) + 1;
        bool any_changed = false;

    std::span<const std::uint8_t> baseMask{_base_mask.data(), _base_mask.size()};
    auto pixels = pixel_view(_damage_image);

        for (int y = std::max(0, py - pr); y <= std::min(_damage_image.height - 1, py + pr); ++y)
        {
            for (int x = std::max(0, px - pr); x <= std::min(_damage_image.width - 1, px + pr); ++x)
            {
                const int dx = x - px;
                const int dy = y - py;
                const float dist = std::sqrt(static_cast<float>((dx * dx) + (dy * dy)));
                if (dist <= static_cast<float>(pr))
                {
                    const float f = 1.0F - (dist / static_cast<float>(pr));
                    const int incr = static_cast<int>(std::ceil(base_alpha_incr * f));
                    int idx = (y * _damage_image.width) + x;
                    if (baseMask.empty() || baseMask[static_cast<std::size_t>(idx)] == 0)
                    {
                        continue; // skip transparent background
                    }
                    auto &pix = pixels[static_cast<std::size_t>(idx)];
                    int old_brightness = static_cast<int>(pix[0]); // RGB should be same for grayscale
                    int new_brightness = std::max(0, old_brightness - incr);
                    if (new_brightness != old_brightness)
                    {
                        // Set RGB to the new brightness level (for multiplicative blending)
                        pix[0] = static_cast<std::uint8_t>(new_brightness);
                        pix[1] = static_cast<std::uint8_t>(new_brightness);
                        pix[2] = static_cast<std::uint8_t>(new_brightness);
                        pix[3] = 255; // keep alpha at 255 for multiplicative blending
                        any_changed = true;
                    }
                }
            }
        }
        if (any_changed)
        {
            _damage_gpu_dirty = true;
            _has_damage = true;
        }
    }
}

void Base::draw(raylib::DrawSession &ds) const
{
    // Default path keeps backward compatibility: draw base, then damage overlay with local blend
    draw_base(ds);
    if (has_damage())
    {
        BeginBlendMode(BLEND_ALPHA);
        draw_damage(ds);
        EndBlendMode();
    }
}

void Base::draw_base(raylib::DrawSession &ds) const
{
    const Rectangle src = {0.0F, 0.0F, _texture.widthF(), _texture.heightF()};
    const Vector2 pos = {_x, _y};
    ds.DrawTexture(_texture.get(), src, pos, GlobalColors::COL_WHITE);
}

void Base::draw_damage(raylib::DrawSession &ds) const
{
    if (!has_damage())
    {
        return;
    }

    const Rectangle damage_src = {0.0F, 0.0F, _damage_tex.widthF(), _damage_tex.heightF()};
    const Vector2 pos = {_x, _y};
    // Semi-transparent black overlay to create visible holes
    const Color hole_color = GlobalColors::withAlpha(GlobalColors::COL_BLACK, 128);
    ds.DrawTexture(_damage_tex.get(), damage_src, pos, hole_color);
}

auto Base::get_hitbox() const noexcept -> Rectangle
{
    // Create hitbox that matches the texture dimensions exactly
    // This ensures proper collision detection with the visual representation
    // Use the actual texture height so the hitbox covers the visible area
    const float height = (_texture.id() > 0) ? _texture.heightF() : GlobalConstant::BASE_SIZE;
    return Rectangle{_x, _y, GlobalConstant::BASE_WIDTH, height};
}
