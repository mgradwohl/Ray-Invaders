#include <array>
#include <chrono>
#include <random>
#include <raylib.h>
#include "RLWindow.h"
#include "RLDrawSession.h"

#include "Animation.hpp"
#include "DrawText.hpp"
#include "Global.hpp"
#include "Enemy.hpp"
#include "EnemyManager.hpp"
#include "Ufo.hpp"
#include "Player.hpp"
#include "PowerUp.hpp"
int main()
{
	bool game_over = 0;
	bool next_level = 0;

	//The current level.
	unsigned short level = 0;
	unsigned short next_level_timer = NEXT_LEVEL_TRANSITION;

	//We'll use this to make the game frame rate independent.
	std::chrono::microseconds lag(0);
	std::chrono::steady_clock::time_point previous_time;

	//Setting a random seed to make sure the random engine will randomly generate random numbers.
	std::mt19937_64 random_engine(std::chrono::system_clock::now().time_since_epoch().count());

	//sf::RenderWindow window(sf::VideoMode(SCREEN_RESIZE * SCREEN_WIDTH, SCREEN_RESIZE * SCREEN_HEIGHT), "Space Invaders", sf::Style::Close);
	raylib::Window window(SCREEN_WIDTH * SCREEN_RESIZE, SCREEN_HEIGHT * SCREEN_RESIZE, 60, "Space Invaders");
	InitAudioDevice();

	Texture2D background_sprite = ::LoadTexture("Resources/Images/Background2.png");

	EnemyManager enemy_manager;

	Player player;
	PowerUp powerup("Resources/Images/PowerupBar.png");

	Ufo ufo(random_engine);

	previous_time = std::chrono::steady_clock::now();

	RenderTexture2D backbuffer = ::LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

	while (!window.ShouldClose())
	{
		//Making the game frame rate independent.
		std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
		lag += delta_time;
		previous_time += delta_time;

		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;

			//We're gonna show the "Game over!" text after the player's death animation.
			if (player.get_dead_animation_over())
			{
				game_over = 1;
			}

			if (enemy_manager.reached_player(player.get_y()))
			{
				player.die();
			}

			if (!game_over)
			{
				if (!enemy_manager.get_enemies().size())
				{
					if (!next_level_timer)
					{
						next_level = 0;

						level++;
						next_level_timer = NEXT_LEVEL_TRANSITION;

						player.reset();

						enemy_manager.reset(level);

						ufo.reset(1, random_engine);
					}
					else //Here we're showing the next level transition.
					{
						next_level = 1;

						next_level_timer--;
					}
				}
				else
				{
					player.update(random_engine, enemy_manager.get_enemy_bullets(), enemy_manager.get_enemies(), ufo);
					powerup.update(player);
					enemy_manager.update(random_engine);
					ufo.update(random_engine);
				}
			}
			else if (IsKeyPressed(KEY_ENTER))
			{
				game_over = 0;
				level = 0;
				player.reset();
				enemy_manager.reset(level);
				ufo.reset(1, random_engine);
			}

			if (FRAME_DURATION > lag)
			{
				{
					raylib::DrawSession ds(backbuffer, BLACK);
					ds.DrawTexture(background_sprite, 0, 0, WHITE);

					//When the player dies, we won't show anything but the player.
					if (!player.get_dead())
					{
						enemy_manager.draw(ds);
						ufo.draw(ds);
						powerup.draw(ds, player);
					}
					player.draw(ds);

					draw_text(ds, 10, 0.25f * BASE_SIZE, 0.25f * BASE_SIZE, "Level: " + std::to_string(level));

					if (game_over)
					{
						//I was too lazy to add center alignment, so I just wrote numbers instead.
						draw_text(ds, 20, 0.5f * (SCREEN_WIDTH - 5 * BASE_SIZE), 0.5f * (SCREEN_HEIGHT - BASE_SIZE), "Game over!");
					}
					else if (next_level)
					{
						draw_text(ds, 20, 0.5f * (SCREEN_WIDTH - 5.5f * BASE_SIZE), 0.5f * (SCREEN_HEIGHT - BASE_SIZE), "Next level!");
					}
				}

				{
					// Draw backbuffer to front buffer
					// -height flips the image the right way up
					Vector2 pos{ 0,0 };
					Rectangle source = { 0, 0, backbuffer.texture.width, -backbuffer.texture.height };

					BeginDrawing();
						Rectangle dest = { 0, 0, backbuffer.texture.width * SCREEN_RESIZE, backbuffer.texture.height * SCREEN_RESIZE };
						DrawTexturePro(backbuffer.texture, source, dest, pos, 0.0f, WHITE);
					EndDrawing();
				}
			}
		}
	}
	UnloadRenderTexture(backbuffer);
}