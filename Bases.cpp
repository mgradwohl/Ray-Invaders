#include <vector>
#include <climits> // For USHRT_MAX
#include <raylib.h>
#include "Bullet.hpp"
#include "Bases.hpp"

Bases::Bases(const std::string& filename)
{	_sprite = LoadTexture(filename.c_str());
	// Calculate framecount directly to int to avoid casting
	_framecount = static_cast<int>(_sprite.width / F::BASE_WIDTH - 1.0f);
	const float offset = (F::SCREEN_WIDTH - (BASE_COUNT * F::BASE_WIDTH)) / (BASE_COUNT + 1);
	float x = offset;
	for (int i = 0; i < BASE_COUNT; i++)
	{
		_bases.emplace_back(x);
		x += offset; // spacing
		x += F::BASE_WIDTH; // BASE WIDTH
	};
}

Bases::~Bases()
{
	UnloadTexture(_sprite);
}

void Bases::reset()
{
	for (Base& base : _bases)
	{
		base.reset();
	}
}

void Bases::update(std::vector<Bullet>& i_enemy_bullets)
{
	// Since _framecount is now an int, we can do a direct comparison
	// without needing intermediate casts
	const unsigned short frameCount = (_framecount > USHRT_MAX) ? 
		USHRT_MAX : static_cast<unsigned short>(_framecount);
		
	for (Base& base : _bases)
	{
		base.update(i_enemy_bullets, frameCount);
	};
}

void Bases::draw(raylib::DrawSession& ds) const
{
	for (const Base& base : _bases)
	{
		base.draw(ds, _sprite);
	};
}