#include <array>
#include <chrono>
#include <random>
#include <raylib.h>
#include "RLWindow.h"
#include "RLDrawSession.h"
#include "Background.hpp"
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

	raylib::Window window(SCREEN_WIDTH * SCREEN_RESIZE, SCREEN_HEIGHT * SCREEN_RESIZE, 60, "Space Invaders");
	InitAudioDevice();

	Background background("Resources/Images/BigGalaxy.png");
	EnemyManager enemy_manager;
	Player player;
	PowerUp powerup("Resources/Images/PowerupBar.png");
	Ufo ufo(random_engine);

	// we draw everything to this, and then render this to the screen
	RenderTexture2D backbuffer = ::LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

	previous_time = std::chrono::steady_clock::now();
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
				// no more enemies left
				if (enemy_manager.get_enemies().size() == 0)
				{
					if (!next_level_timer)
					{
						next_level = 0;
						level++;
						next_level_timer = NEXT_LEVEL_TRANSITION;
						background.reset();
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
					// enemies left, update everything
					player.update(random_engine, enemy_manager.get_enemy_bullets(), enemy_manager.get_enemies(), ufo);
					powerup.update(player);
					background.update(player);
					enemy_manager.update(random_engine);
					ufo.update(random_engine);
				}
			}
			else if (IsKeyPressed(KEY_ENTER))
			{
				// player started a new game
				game_over = 0;
				level = 0;
				background.reset();
				player.reset();
				enemy_manager.reset(level);
				ufo.reset(1, random_engine);
			}

			if (FRAME_DURATION > lag)
			{
				{
					// everything is either reset (new game) or updated (continuing game) time to draw
					raylib::DrawSession ds(backbuffer, BLACK);
					background.draw(ds);

					// When the player dies, we will only the player and the banner
					player.draw(ds);
					if (!player.get_dead())
					{
						enemy_manager.draw(ds);
						ufo.draw(ds);
						powerup.draw(ds, player);
						draw_text(ds, 10, 0.25f * BASE_SIZE, 0.25f * BASE_SIZE, "Level: " + std::to_string(level));
					}
					else
					{
						draw_text(ds, 20, 0.5f * (SCREEN_WIDTH - 5 * BASE_SIZE), 0.5f * (SCREEN_HEIGHT - BASE_SIZE), "Game over!");
					}

					if (next_level)
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