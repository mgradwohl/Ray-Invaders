#include <vector>
#include <raylib.h>
#include "Bullet.hpp"
#include "Bases.hpp"

Bases::Bases(const std::string& filename)
{
	_sprite = LoadTexture(filename.c_str());

	unsigned short x = 40;
	for (int i = 0; i < BASE_COUNT; i++)
	{
		_bases.emplace_back(x);
		x += 40;			// spacing
		x += BASE_WIDTH;	// BASE WIDTH
	};
}

Bases::~Bases()
{
	UnloadTexture(_sprite);
}

void Bases::reset(bool dead)
{
	for (Base& base : _bases)
	{
		base.reset();
	};
}

void Bases::update(std::vector<Bullet>& i_enemy_bullets)
{
	for (Base& base : _bases)
	{
		base.update(i_enemy_bullets);
	};
}

void Bases::draw(raylib::DrawSession& ds)
{
	for (Base& base : _bases)
	{
		base.draw(ds, _sprite);
	};
}