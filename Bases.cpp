#include <vector>
#include <raylib.h>
#include "Bullet.hpp"
#include "Bases.hpp"

Bases::Bases(const std::string& filename)
{
	_sprite = LoadTexture(filename.c_str());
	_framecount = static_cast<unsigned short>(_sprite.width / BASE_WIDTH - 1);

	constexpr unsigned short offset = static_cast<unsigned short>((SCREEN_WIDTH - (BASE_COUNT * BASE_WIDTH)) / (BASE_COUNT + 1));
	unsigned short x = offset;
	for (int i = 0; i < BASE_COUNT; i++)
	{
		_bases.emplace_back(x);
		x = static_cast<unsigned short>(x + offset); // spacing
		x = static_cast<unsigned short>(x + BASE_WIDTH); // BASE WIDTH
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
	for (Base& base : _bases)
	{
		base.update(i_enemy_bullets, _framecount);
	};
}

void Bases::draw(raylib::DrawSession& ds)
{
	for (Base& base : _bases)
	{
		base.draw(ds, _sprite);
	};
}