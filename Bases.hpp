#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"
#include "Base.hpp"

class Bases
{
public:
	Bases(const std::string& filename);
	~Bases();

	void reset(bool dead);
	void update(std::vector<Bullet>& i_enemy_bullets);
	void draw(raylib::DrawSession& ds);

private:
	std::vector<Base> _bases;
	unsigned short _framecount = 0;

	Texture2D _sprite;
};