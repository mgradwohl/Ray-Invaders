#pragma once
#include <string>
#include <raylib.h>
#include "RLDrawSession.h"

void draw_text(raylib::DrawSession& ds, short size, short i_x, short i_y, const std::string& i_text);