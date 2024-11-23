#pragma once
#include <string>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"

class Base
{
public:
	Base(unsigned short x);
	~Base();
	void reset();
	void update(std::vector<Bullet>& i_enemy_bullets);
	void draw(raylib::DrawSession& ds, Texture2D& sprite);

	Rectangle get_hitbox() const;

private:
	unsigned short damage = 0;
	bool dead = false;
	unsigned short _x = 0;
	unsigned short _y = 0;
	unsigned short frame = 0;
};

