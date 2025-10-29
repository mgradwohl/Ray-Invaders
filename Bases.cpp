// Corresponding header
#include "Bases.hpp"

// Standard library headers
#include <vector>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Bullet.hpp"
#include "HitManager.hpp"

Bases::Bases(const std::string &filename)
{
    // Load the base image and calculate framecount
    _baseImage = LoadImage(filename.c_str());
    _framecount = static_cast<int>((_baseImage.width / GlobalConstant::BASE_WIDTH) - 1.0F);

    // Calculate positioning for bases
    const float offset = (GlobalConstant::SCREEN_WIDTH - (GlobalConstant::Int::BASE_COUNT * GlobalConstant::BASE_WIDTH)) /
                         (GlobalConstant::Int::BASE_COUNT + 1);
    float x = offset;

    // Create empty bases at the correct positions first
    _bases.reserve(GlobalConstant::Int::BASE_COUNT);
    for (int i = 0; i < GlobalConstant::Int::BASE_COUNT; i++)
    {
        _bases.emplace_back(std::make_unique<Base>(x));
        x += offset;                     // spacing
        x += GlobalConstant::BASE_WIDTH; // BASE WIDTH
    }

    // Reset all bases with the base image
    reset();
}

Bases::~Bases()
{
    UnloadImage(_baseImage);
    // Base destructors will unload their own textures
}

void Bases::reset()
{
    for (auto &base : _bases)
    {
        base->reset(_baseImage);
    }
}

void Bases::update(std::vector<Bullet> &i_bullets, HitManager &hits)
{
    // Since _framecount is now an int, we can do a direct comparison
    // without needing intermediate casts
    const GameTypes::Count frameCount = (_framecount > UINT8_MAX) ? UINT8_MAX : static_cast<GameTypes::Count>(_framecount);

    for (auto &base : _bases)
    {
        base->update(i_bullets, frameCount, hits);
    }
}

void Bases::draw(raylib::DrawSession &ds) const
{
    // First pass: draw all base textures
    for (const auto &base : _bases)
    {
        base->draw_base(ds);
    }
    // Second pass: batch damage overlays under a single blend mode for fewer state changes
    bool any_damage = false;
    for (const auto &base : _bases)
    {
        if (base->has_damage())
        {
            any_damage = true;
            break;
        }
    }
    if (any_damage)
    {
        BeginBlendMode(BLEND_ALPHA);
        for (const auto &base : _bases)
        {
            base->draw_damage(ds);
        }
        EndBlendMode();
    }
}
