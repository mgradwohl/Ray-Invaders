#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"

class Base
{
public:
	Base() = default;
	Base(unsigned short x) noexcept;
	~Base() = default;
	void reset() noexcept;
	void update(std::vector<Bullet>& i_enemy_bullets, const unsigned short framecount);
	void draw(raylib::DrawSession& ds, const Texture2D& sprite);

	[[nodiscard]] Rectangle get_hitbox() const noexcept;

private:
	unsigned short _damage = 0;
	bool _dead = false;
	unsigned char _frame = 0;
	float _x = 0;
	float _y = 0;
};

