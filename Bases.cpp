// Corresponding header
#include "Bases.hpp"

// Standard library headers
#include <vector>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Bullet.hpp"
#include "HitManager.hpp"

Bases::Bases(const std::string& filename)
{
    // Load the base image and calculate framecount
    _baseImage = LoadImage(filename.c_str());
    _framecount = static_cast<int>(_baseImage.width / GlobalConstant::BASE_WIDTH - 1.0f);
    
    // Calculate positioning for bases
    const float offset = (GlobalConstant::SCREEN_WIDTH - (GlobalConstant::Int::BASE_COUNT * GlobalConstant::BASE_WIDTH)) / (GlobalConstant::Int::BASE_COUNT + 1);
    float x = offset;
    
    // Create empty bases at the correct positions first
    for (int i = 0; i < GlobalConstant::Int::BASE_COUNT; i++)
    {
        _bases.emplace_back(x);
        x += offset; // spacing
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
    for (std::size_t i = 0; i < _bases.size(); ++i)
    {
        _bases[i].reset(_baseImage);
    }
}

void Bases::update(std::vector<Bullet>& i_bullets, HitManager& hits)
{
    // Since _framecount is now an int, we can do a direct comparison
    // without needing intermediate casts
    const GameTypes::Count frameCount = (_framecount > UINT8_MAX) ? 
        UINT8_MAX : static_cast<GameTypes::Count>(_framecount);
        
    for (Base& base : _bases)
    {
        base.update(i_bullets, frameCount, hits);
    }
}

void Bases::draw(raylib::DrawSession& ds) const
{
    for (const Base& base : _bases)
    {
        base.draw(ds);
    }
}





