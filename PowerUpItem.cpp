// Corresponding header
#include "PowerUpItem.hpp"

// Third-party headers
#include <raylib.h>

// Project headers
#include "Global.hpp"

PowerUpItem::PowerUpItem(float i_x, float i_y, Type i_type) noexcept :
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
	return Rectangle(_x + 0.25F * GlobalConstant::BASE_SIZE, _y + 0.25F * GlobalConstant::BASE_SIZE, 0.5F * GlobalConstant::BASE_SIZE, 0.5F * GlobalConstant::BASE_SIZE);
}

PowerUpItem::Type PowerUpItem::get_type() const noexcept
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

float PowerUpItem::get_x() const noexcept 
{ 
	return _x; 
}

float PowerUpItem::get_y() const noexcept 
{ 
	return _y; 
}





