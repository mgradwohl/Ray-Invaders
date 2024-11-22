#pragma once
#include <string>
#include <raylib.h>
#include "RLWindow.h"
#include "RLDrawSession.h"

class Animation
{
public:
	Animation(unsigned short i_animation_speed, unsigned short i_frame_width, const std::string& i_texture_location);

	bool change_current_frame();
	bool update();

	void draw(raylib::DrawSession& ds, short i_x, short i_y, const Color& i_color = WHITE);
	void reset();

private:
	//When this iterator reaches the animation speed, we change the frame and reset the iterator.
	unsigned short animation_iterator;
	//The higher the value, the slower the animation.
	unsigned short animation_speed;
	unsigned short current_frame;
	//To make things easier, each image file will contain 1 animation. So that the frame heights are the same.
	unsigned short frame_width;
	//We can find this by dividing the width of the image by the frame width.
	unsigned short total_frames;

	Texture2D sprite;
};