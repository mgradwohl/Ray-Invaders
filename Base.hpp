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
	void update(std::vector<Bullet>& i_enemy_bullets, unsigned short framecount);
	void draw(raylib::DrawSession& ds, Texture2D& sprite);

	Rectangle get_hitbox() const;

private:
	unsigned short _damage = 0;
	bool _dead = false;
	float _x = 0;
	float _y = 0;
	unsigned short _frame = 0;
};

