#pragma once
#include <string>
#include <raylib.h>
#include "RLWindow.h"
#include "RLDrawSession.h"

class Animation
{
public:
	Animation(unsigned short i_animation_speed, unsigned short i_frame_width, const std::string& i_texture_location);

	bool change_current_frame() noexcept;
	bool update() noexcept;

	void draw(raylib::DrawSession& ds, float x, float y, const Color& i_color = WHITE) const;
	void reset();

private:
	//When this iterator reaches the animation speed, we change the frame and reset the iterator.
	unsigned short _animation_iterator;
	//The higher the value, the slower the animation.
	unsigned short _animation_speed;
	unsigned short _current_frame;
	//To make things easier, each image file will contain 1 animation. So that the frame heights are the same.
	unsigned short _frame_width;
	//We can find this by dividing the width of the image by the frame width.
	unsigned short _total_frames;

	Texture2D _sprite;
};