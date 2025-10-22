// Standard library headers
#include <chrono>
#include <cstdlib>
#include <random>
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Backbuffer.hpp"
#include "Background.hpp"
#include "BannerUI.hpp"
#include "Bases.hpp"
#include "EnemyManager.hpp"
#include "Global.hpp"
#include "HitManager.hpp"
#include "Player.hpp"
#include "PowerUpManager.hpp"
#include "RLDrawSession.h"
#include "RLWindow.h"
#include "Ufo.hpp"

auto main() -> int
{
    bool game_over = false;
    bool next_level = false;
    GameTypes::Level level = 1;
    GameTypes::Timer next_level_timer = GlobalConstant::Int::NEXT_LEVEL_TRANSITION;

    // We'll use this to make the game frame rate independent.
    std::chrono::microseconds lag(0);
    std::chrono::steady_clock::time_point previous_time;

    // Setting a random seed to make sure the random engine will randomly generate random numbers.
    std::mt19937_64 random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    raylib::Window const window(
        GlobalConstant::Int::SCREEN_WIDTH * GlobalConstant::Int::SCREEN_RESIZE,
        GlobalConstant::Int::WINDOW_HEIGHT * GlobalConstant::Int::SCREEN_RESIZE, 60,
        "Ray Invaders");

    Background background("Resources/Images/BigGalaxy.png");
    EnemyManager enemy_manager;
    HitManager hit_manager;
    Player player;
    PowerUpManager powerup("Resources/Images/PowerupBar.png");
    Ufo ufo(random_engine);
    Bases bases("Resources/Images/Base.png");

    // AUTO_FIRE debug helper removed: spawning enemy bullets at startup was
    // intrusive during regular testing and is no longer desirable.

    // we draw everything to this, and then render this to the screen
    Backbuffer backbuffer(GlobalConstant::Int::SCREEN_WIDTH, GlobalConstant::Int::SCREEN_HEIGHT,
                          GlobalConstant::Int::SCREEN_RESIZE);
    BannerUI bannerUI(&powerup);

    previous_time = std::chrono::steady_clock::now();
    while (!window.ShouldClose())
    {
        // Making the game frame rate independent.
        const std::chrono::microseconds delta_time =
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -
                                                                  previous_time);
        lag += delta_time;
        previous_time += delta_time;

        while (GlobalConstant::Time::FRAME_DURATION <= lag)
        {
            lag -= GlobalConstant::Time::FRAME_DURATION;

            // We're gonna show the "Game over!" text after the player's death animation.
            if (player.get_dead_animation_over())
            {
                game_over = 1;
            } // Player's Y position is already a float, so we can check directly
            if (enemy_manager.reached_player(player.get_y()))
            {
                player.die();
            }

            if (!game_over)
            {
                // no more enemies left
                if (enemy_manager.get_enemies().empty())
                {
                    if (!next_level_timer)
                    {
                        next_level = 0;
                        level++;
                        next_level_timer = GlobalConstant::Int::NEXT_LEVEL_TRANSITION;
                        background.reset();
                        player.reset();
                        enemy_manager.reset(level);
                        ufo.reset(1, random_engine);
                        bases.reset();
                    }
                    else // Here we're showing the next level transition.
                    {
                        next_level = 1;
                        next_level_timer--;
                    }
                }
                else
                {
                    // enemies left, update everything
                    player.update(random_engine, enemy_manager.get_enemy_bullets(),
                                  enemy_manager.get_enemies(), ufo, hit_manager);
                    powerup.update(player);
                    background.update(player);
                    enemy_manager.update(random_engine);
                    ufo.update(random_engine);
                    bases.update(enemy_manager.get_enemy_bullets(), hit_manager);
                    bases.update(player.get_player_bullets(), hit_manager);
                }
            }
            else if (IsKeyPressed(KEY_ENTER))
            {
                // player started a new game
                game_over = false;
                level = 0;
                background.reset();
                player.reset();
                enemy_manager.reset(level);
                ufo.reset(1, random_engine);
                bases.reset();
            }

            // Always age out hits each tick, regardless of game state (transition, game over, etc.)
            hit_manager.update();

            if (GlobalConstant::Time::FRAME_DURATION > lag)
            {
                {
                    // Draw gameplay into gameplay render texture
                    raylib::DrawSession dsGameplay(backbuffer.GetGameplayRenderTexture(), BLACK);
                    background.draw(dsGameplay);
                    player.draw(dsGameplay);
                    if (!player.get_dead())
                    {
                        enemy_manager.draw(dsGameplay);
                        ufo.draw(dsGameplay);
                        bases.draw(dsGameplay);
                        hit_manager.draw(dsGameplay);
                    }
                    else
                    {
                        dsGameplay.DrawTextCentered("Game over!",
                                                    GlobalConstant::Int::SCREEN_WIDTH / 2,
                                                    GlobalConstant::Int::SCREEN_HEIGHT / 2,
                                                    GlobalConstant::Int::FONT_SIZE_BIG, WHITE);
                    }
                    if (next_level)
                    {
                        dsGameplay.DrawTextCentered("Next level!",
                                                    GlobalConstant::Int::SCREEN_WIDTH / 2,
                                                    GlobalConstant::Int::SCREEN_HEIGHT / 2,
                                                    GlobalConstant::Int::FONT_SIZE_BIG, WHITE);
                    }
                } // dsGameplay
                {
                    // Draw banner into banner render texture
                    raylib::DrawSession dsBanner(backbuffer.GetBannerRenderTexture(), BLACK);
                    bannerUI.setLevel(level);
                    bannerUI.draw(dsBanner, player);
                } // dsBanner
                backbuffer.flip();
            }
        }
    }
}
