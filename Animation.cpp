//I didn't use the constants from "Global.hpp" to make this class more universal.
#include <raylib.h>
#include "RLDrawSession.h"

#include "Animation.hpp"

Animation::Animation(GameTypes::Speed i_animation_speed, GameTypes::Size i_frame_width, const std::string& i_texture_location) noexcept :
	_animation_iterator(0),
	_animation_speed(std::max(1, static_cast<int>(i_animation_speed))),
	_current_frame(0),
	// Convert frame width to float during initialization
	_frame_width(static_cast<float>(i_frame_width))
{
	_sprite = ::LoadTexture(i_texture_location.c_str());

	// Calculate once and reuse
	const float spriteWidthF = static_cast<float>(_sprite.width);
	// No need for cast to unsigned short, _total_frames is now int
	_total_frames = static_cast<int>(spriteWidthF / _frame_width);
}

//This is for the enemies.
bool Animation::change_current_frame() noexcept
{
	_current_frame++;

	if (_current_frame == _total_frames)
	{
		_current_frame = 0;

		return true;
	}

	return false;
}

bool Animation::update() noexcept
{
	bool output = false;

	_animation_iterator++;

	while (_animation_iterator >= _animation_speed)
	{
		_animation_iterator -= _animation_speed;
		_current_frame++;

		if (_current_frame == _total_frames)
		{
			output = true;

			_current_frame = 0;
		}
	}

	return output;
}

void Animation::draw(raylib::DrawSession& ds, float x, float y, const Color& i_color) const
{
	const Vector2 dest{ x, y };
	// Cache the height conversion to avoid repeated casts
	const float spriteHeight = static_cast<float>(_sprite.height);
	const Rectangle source{ _current_frame * _frame_width, 0.0f, _frame_width, spriteHeight };
	const Color ani{ i_color.r, i_color.g, i_color.b, 255 };
	ds.DrawTexture(_sprite, source, dest, ani);
}

void Animation::reset() noexcept
{
	_animation_iterator = 0;
	_current_frame = 0;
}