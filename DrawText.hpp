#pragma once
#include <string>
#include <raylib.h>
#include "RLDrawSession.h"

void draw_text(raylib::DrawSession& ds, short size, short x, short y, const std::string& text);
void draw_text_center(raylib::DrawSession& ds, short size, unsigned short width, unsigned short height, const std::string& text);