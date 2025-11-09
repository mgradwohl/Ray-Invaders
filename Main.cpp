// Standard library headers
#include <chrono>
// #include <cstdlib>
#include "Random.hpp"
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
#include "RLDrawSession.hpp"
#include "RLWindow.hpp"
#include "Ufo.hpp"
#include "Popup.hpp"

auto main() -> int
{
    bool game_over = false;
    GameTypes::Level level = 1;
    GameTypes::Timer next_level_timer = GlobalConstant::Int::NEXT_LEVEL_TRANSITION;

    // We'll use this to make the game frame rate independent.
    std::chrono::microseconds lag(0);
    std::chrono::steady_clock::time_point previous_time;

    // Seed XOSHIRO random generator
    Random::seed(std::chrono::system_clock::now().time_since_epoch().count());

    raylib::Window const window(GlobalConstant::Int::SCREEN_WIDTH * GlobalConstant::Int::SCREEN_RESIZE,
                                GlobalConstant::Int::WINDOW_HEIGHT * GlobalConstant::Int::SCREEN_RESIZE, 60, "Ray Invaders");

    Background background("Resources/Images/BigGalaxy.png");
    EnemyManager enemy_manager;
    HitManager hit_manager;
    Player player;
    PowerUpManager powerup("Resources/Images/PowerupBar.png");
    Ufo ufo;
    Bases bases("Resources/Images/Base2.png");
    Popup popupNextLevel;
    popupNextLevel.loadFromFile("Resources/Images/PopupNextLevel.png");
    Popup popupGameOver;
    popupGameOver.loadFromFile("Resources/Images/PopupGameOver.png");

    // we draw everything to this, and then render this to the screen
    Backbuffer backbuffer(GlobalConstant::Int::SCREEN_WIDTH, GlobalConstant::Int::SCREEN_HEIGHT, GlobalConstant::Int::SCREEN_RESIZE);
    BannerUI bannerUI(&powerup);

    previous_time = std::chrono::steady_clock::now();
    while (!window.ShouldClose())
    {
        // Making the game frame rate independent.
        const auto delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
        lag += delta_time;
        previous_time += delta_time;

        while (GlobalConstant::Time::FRAME_DURATION <= lag)
        {
            lag -= GlobalConstant::Time::FRAME_DURATION;

            // Update popups each tick so their timers/inputs progress even when they are blocking the game world.
            popupNextLevel.update();
            popupGameOver.update();
            // Always age out hits each tick, regardless of game state (transition, game over, etc.)
            hit_manager.update();

            // Player's Y position is already a float, so we can check directly
            // This should be moved to enemy_manager.update or similar
            if (enemy_manager.reached_player(player.get_y()))
            {
                player.die();
            }

            // This could probably be moved to player.update, but for keep it here for now    
            // We're gonna show the "Game over!" text after the player's death animation.
            if (player.get_dead_animation_over())
            {
                // If player has remaining lives, consume one and respawn; otherwise enter game over as before
                if (player.get_lives() > 1)
                {
                    // Consume a life and reset player for respawn
                    player.lose_life();
                    player.reset();
                }
                else
                {
                    game_over = true;
                }
            }
            // this should be in a new class called Game, and happen during Game::Update
            if (!game_over)
            {
                // no more enemies left
                if (enemy_manager.get_enemies().empty())
                {
                    if (next_level_timer == 0U)
                    {
                        level++;
                        popupNextLevel.hide();
                        next_level_timer = GlobalConstant::Int::NEXT_LEVEL_TRANSITION;
                        background.reset();
                        player.reset();
                        enemy_manager.reset(level);
                        ufo.reset(true);
                        bases.reset();
                    }
                    else // Here we're showing the next level transition.
                    {
                        // Show a non-interactive popup synced with the timer. The show
                        // call is idempotent while active.
                        popupNextLevel.show(0, true, true);
                        next_level_timer--;
                    }
                }
                else
                {
                    // there are still enemies, update everything
                    // If a blocking popup is active, skip gameplay updates
                    const bool popupBlockingActive = (popupNextLevel.isActive() && popupNextLevel.isBlocking()) ||
                                                     (popupGameOver.isActive() && popupGameOver.isBlocking());

                    if (!popupBlockingActive)
                    {
                        // enemies left, update everything
                        player.update(enemy_manager.get_enemy_bullets(), enemy_manager.get_enemies(), ufo, hit_manager);
                        powerup.update(player);
                        background.update(player);
                        enemy_manager.update();
                        ufo.update();
                        bases.update(enemy_manager.get_enemy_bullets(), hit_manager);
                        bases.update(player.get_player_bullets(), hit_manager);
                    }
                    else
                    {
                        // While a blocking popup is active we intentionally do NOT advance
                        // the game world state (enemies, bullets, bases). Only advance
                        // player animations (and other minor visual-only updates). To
                        // avoid processing new hits that would put enemies into a
                        // half-dead state (hit timer set but no enemy.update to finish
                        // the death), call player.update with empty enemy/bullet lists.

                        // todo refine this, is it really a problem?
                        // Or should we just empty the real vectors when we show a popup
                        std::vector<Bullet> empty_enemy_bullets;
                        std::vector<Enemy> empty_enemies;
                        player.update(empty_enemy_bullets, empty_enemies, ufo, hit_manager);
                    }
                }
            }

            // game_over should be a state inside a Game class
            // over, paused, playing, transitioning, etc.
            if (game_over)
            {
                // Show blocking game-over popup that waits for input
                popupGameOver.show(0, true, true);

                if (IsKeyPressed(KEY_ENTER))
                {
                    popupGameOver.hide();
                    // player started a new game
                    game_over = false;
                    level = 0;
                    background.reset();
                    // Reset player's lives to initial value on a fresh game
                    player.set_lives(GlobalConstant::Int::INITIAL_LIVES);
                    player.reset();
                    enemy_manager.reset(level);
                    ufo.reset(true);
                    bases.reset();
                }
            }

            // draw everything if we have time left this frame
            if (GlobalConstant::Time::FRAME_DURATION > lag)
            {
                {
                    // Draw gameplay into gameplay render texture
                    raylib::DrawSession dsGameplay(backbuffer.GetGameplayRenderTexture(), BLACK);
                    background.draw(dsGameplay);
                    player.draw(dsGameplay);

                    // Draw world entities while the game is active. The "Game over!"
                    // message is shown only when the game_over flag is true (i.e. the
                    // player has no lives left and the session ended).
                    if (!game_over)
                    {
                        enemy_manager.draw(dsGameplay);
                        ufo.draw(dsGameplay);
                        bases.draw(dsGameplay);
                        hit_manager.draw(dsGameplay);
                    }

                    // Draw active popups (they render centered on the screen). If a
                    // popup isn't active we fall back to the old text draw.
                    popupNextLevel.draw(dsGameplay);
                    popupGameOver.draw(dsGameplay);

                } // dsGameplay

                {
                    // Draw banner into banner render texture
                    raylib::DrawSession dsBanner(backbuffer.GetBannerRenderTexture(), GlobalColors::COL_BLACK);
                    bannerUI.setLevel(level);
                    bannerUI.draw(dsBanner, player);
                } // dsBanner
                backbuffer.flip();
            }
        }
    }
}
