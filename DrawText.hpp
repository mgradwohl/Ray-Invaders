#pragma once
#include <string>
#include <raylib.h>
#include "RLDrawSession.h"

void draw_text(raylib::DrawSession& ds, short size, short i_x, short i_y, const std::string& i_text);
void draw_text_center(raylib::DrawSession& ds, short size, unsigned short width, unsigned short height, const std::string& i_text);