#pragma once
#include <string>
#include <raylib.h>
#include "RLDrawSession.h"

void draw_text(short i_x, short i_y, const std::string& i_text, raylib::DrawSession& ds, const Texture2D& i_font_texture);