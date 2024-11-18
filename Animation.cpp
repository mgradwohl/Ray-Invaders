//I didn't use the constants from "Global.hpp" to make this class more universal.
#include <raylib.h>
#include "RLDrawSession.h"

#include "Animation.hpp"

Animation::Animation(unsigned short i_animation_speed, unsigned short i_frame_width, const std::string& i_texture_location) :
	animation_iterator(0),
	animation_speed(std::max<unsigned short>(1, i_animation_speed)),
	current_frame(0),
	frame_width(i_frame_width)
{
	sprite = ::LoadTexture(i_texture_location.c_str());

	total_frames = sprite.width / frame_width;
}

//This is for the enemies.
bool Animation::change_current_frame()
{
	current_frame++;

	if (current_frame == total_frames)
	{
		current_frame = 0;

		return 1;
	}

	return 0;
}

bool Animation::update()
{
	bool output = 0;

	animation_iterator++;

	while (animation_iterator >= animation_speed)
	{
		animation_iterator -= animation_speed;
		current_frame++;

		if (current_frame == total_frames)
		{
			output = 1;

			current_frame = 0;
		}
	}

	return output;
}

void Animation::draw(raylib::DrawSession& ds, short i_x, short i_y,  const Color& i_color)
{
	//I added coloring for the explosions.
	//sprite.setColor(i_color);
	//sprite.setPosition(i_x, i_y);
	//sprite.setTexture(texture);
	//sprite.setTextureRect(sf::IntRect(current_frame * frame_width, 0, frame_width, texture.getSize().y));
	//i_window.draw(sprite);

	Vector2 dest{ i_x, i_y };
	Rectangle source{current_frame * frame_width, 0.0f, frame_width, sprite.height };
	Color ani{ i_color.r, i_color.g, i_color.b, 255 };
	ds.DrawTexture(sprite, source, dest, ani);
}

void Animation::reset()
{
	animation_iterator = 0;
	current_frame = 0;
}