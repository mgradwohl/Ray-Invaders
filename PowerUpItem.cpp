#include <raylib.h>
#include "PowerUpItem.hpp"

PowerUpItem::PowerUpItem(float i_x, float i_y, GameTypes::Count i_type) noexcept :
	_x(i_x),
	_y(i_y),
	_dead(false),
	_type(i_type)
{
	
}

void PowerUpItem::bump_y(GameTypes::Distance ybump) noexcept
{
	_y += ybump;
}

[[nodiscard]] Rectangle PowerUpItem::get_hitbox() const noexcept
{
	return Rectangle(_x + 0.25F * BASE_SIZE, _y + 0.25F * BASE_SIZE, 0.5F * BASE_SIZE, 0.5F * BASE_SIZE);
}

GameTypes::Count PowerUpItem::get_type() const noexcept
{
	return _type;
}

bool PowerUpItem::isdead() const noexcept 
{ 
	return _dead; 
}

void PowerUpItem::isdead(bool dead) noexcept 
{ 
	_dead = dead; 
}

float PowerUpItem::getx() const noexcept 
{ 
	return _x; 
}

float PowerUpItem::gety() const noexcept 
{ 
	return _y; 
}
