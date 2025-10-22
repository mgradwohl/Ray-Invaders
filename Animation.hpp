#pragma once
#include <string>

#include <raylib.h>

#include "Global.hpp"
#include "RLDrawSession.h"
#include "RLTexture2D.hpp"

class Animation
{
public:
	Animation(GameTypes::Speed i_animation_speed, GameTypes::Size i_frame_width, const std::string& i_texture_location) noexcept;

    auto change_current_frame() noexcept -> bool;
    auto update() noexcept -> bool;

    void draw(raylib::DrawSession& ds, float x, float y, const Color& i_color = WHITE) const;
	void reset() noexcept;

private:
	//When this iterator reaches the animation speed, we change the frame and reset the iterator.
	int _animation_iterator{0};
	//The higher the value, the slower the animation.
	int _animation_speed{1};
	int _current_frame{0};
	//To make things easier, each image file will contain 1 animation. So that the frame heights are the same.
    float _frame_width{0.0F}; // Using float for consistent positioning
    // We can find this by dividing the width of the image by the frame width.
    int _total_frames{0};

	raylib::Texture2DFile _sprite;
};