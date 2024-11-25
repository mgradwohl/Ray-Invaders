//I didn't use the constants from "Global.hpp" to make this class more universal.
#include <raylib.h>
#include "RLDrawSession.h"

#include "Animation.hpp"

Animation::Animation(unsigned short i_animation_speed, unsigned short i_frame_width, const std::string& i_texture_location) :
	_animation_iterator(0),
	_animation_speed(std::max<unsigned short>(1, i_animation_speed)),
	_current_frame(0),
	_frame_width(i_frame_width)
{
	_sprite = ::LoadTexture(i_texture_location.c_str());

	_total_frames = _sprite.width / _frame_width;
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

void Animation::draw(raylib::DrawSession& ds, float x, float y, const Color& i_color)
{
	//I added coloring for the explosions.
	//sprite.setColor(i_color);
	//sprite.setPosition(i_x, i_y);
	//sprite.setTexture(texture);
	//sprite.setTextureRect(sf::IntRect(current_frame * frame_width, 0, frame_width, texture.getSize().y));
	//i_window.draw(sprite);

	const Vector2 dest{ x, y };
	const Rectangle source{_current_frame * _frame_width, 0.0f, _frame_width, _sprite.height };
	const Color ani{ i_color.r, i_color.g, i_color.b, 255 };
	ds.DrawTexture(_sprite, source, dest, ani);
}

void Animation::reset()
{
	_animation_iterator = 0;
	_current_frame = 0;
}