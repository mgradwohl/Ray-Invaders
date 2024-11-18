#include <string>
#include <raylib.h>
#include "RLWindow.h"
#include "DrawText.hpp"
#include "RLDrawSession.h"

void draw_text(raylib::DrawSession& ds, short size, short i_x, short i_y, const std::string& i_text)
{
	DrawText(i_text.c_str(), i_x, i_y, size, WHITE);
}

//void draw_text2(short i_x, short i_y, const std::string& i_text, raylib::DrawSession& ds, const Texture2D& i_font_texture)
//{
//	short character_x = i_x;
//	short character_y = i_y;
//
//	unsigned char character_height = i_font_texture.height;
//	//There are 96 characters in the font texture.
//	unsigned char character_width = i_font_texture.width / 96;
//
//	Texture2D character_sprite(i_font_texture);
//
//	Rectangle source{ 0,0, character_width, character_height };
//	Vector2 dest{ character_x, character_y };
//
//	for (std::string::const_iterator a = i_text.begin(); a != i_text.end(); a++)
//	{
//		if ('\n' == *a)
//		{
//			character_x = i_x;
//			character_y += character_height;
//
//			continue;
//		}
//
//		//character_sprite.setPosition(character_x, character_y);
//		//character_sprite.setTextureRect(sf::IntRect(character_width * (*a - 32), 0, character_width, character_height));
//		//character_x += character_width;
//		//i_window.draw(character_sprite);
//
//		dest.x = character_x;
//		dest.y = character_y;
//		source.x = character_width * (*a - 32);
//		source.y = 0;
//		character_x += character_width;
//
//		ds.DrawTexture(character_sprite, source, dest, WHITE);
//	}
//}

