// Corresponding header
#include "EnemyManager.hpp"

// Standard library headers
#include <algorithm>
#include <iterator>
#include <string>

// Third-party headers
#include <raylib.h>

// Project headers
#include "Animation.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"

// There are 8 levels. Once the player finishes level 8, we go back to level 4. This is the same
// thing we did in the game "Frogger".
//  Fine-tune: Base hitbox is now 93.3% of BASE_WIDTH (based on sprite bitmap analysis), centered
//  for precise collision detection.
EnemyManager::EnemyManager() noexcept
    : _shoot_distribution(0, GlobalConstant::Int::ENEMY_SHOOT_CHANCE),
      _enemy_bullet_sprite("Resources/Images/EnemyBullet.png"),
      _enemymove("Resources/Sounds/Enemy Move.wav"),
      _enemydestroy("Resources/Sounds/Enemy Destroy.wav")
{
    // We have a function that sets everything to the initial state, so why not use it?
    reset(0);

    for (GameTypes::Count a = 0; a < GlobalConstant::Int::ENEMY_TYPES; a++)
    {
        // Explicitly convert to match Animation constructor parameter
        const int animSpeed = 1 + _move_pause;
        const std::string filename = "Resources/Images/Enemy" + std::to_string(a) + ".png";
        _enemy_animations.emplace_back(static_cast<unsigned short>(animSpeed),
                                       GlobalConstant::Int::BASE_SIZE, filename);
    }
}

bool EnemyManager::reached_player(float i_player_y) const
{
    for (const Enemy &enemy : _enemies)
    {
        if (enemy.get_y() > i_player_y - 0.5f * GlobalConstant::BASE_SIZE)
        {
            // As soon as the enemies reach the player, the game is over!
            return true;
        }
    }

    return false;
}

void EnemyManager::draw(raylib::DrawSession &ds) const
{
    for (const Bullet &bullet : _enemy_bullets)
    { // Drawing the tail of the bullet.
        const auto &prev_x = bullet.get_previous_x();
        const auto &prev_y = bullet.get_previous_y();

        // Calculate this once for better performance and readability
        const size_t tailSize = prev_x.size();

        for (std::size_t a = 0; a < tailSize; a++)
        {
            const Vector2 dest{prev_x[a], prev_y[a]}; // prev_x and prev_y are already float
            // Array index 'a' will be small, implicit conversion is fine here
            const float sourceX = GlobalConstant::BASE_SIZE * a;
            const Rectangle source{sourceX, 0.0f, GlobalConstant::BASE_SIZE,
                                   GlobalConstant::BASE_SIZE};
            ds.DrawTexture(_enemy_bullet_sprite.get(), source, dest, WHITE);
        }

        // Drawing the bullet itself.
        const Vector2 dest{bullet.get_x(), bullet.get_y()};
        const float sourceX =
            GlobalConstant::BASE_SIZE * tailSize; // tailSize is small, implicit conversion is fine
        const Rectangle source{sourceX, 0.0f, GlobalConstant::BASE_SIZE, GlobalConstant::BASE_SIZE};
        ds.DrawTexture(_enemy_bullet_sprite.get(), source, dest, WHITE);
    }
    for (const Enemy &enemy : _enemies)
    {
        Color enemy_color = WHITE;

        // Determine the base color of the enemy
        switch (enemy.get_type())
        {
        case Enemy::Type::Cyan:
        {
            enemy_color = Color{0, 255, 255, 255}; // CYAN
            break;
        }
        case Enemy::Type::Purple:
        {
            enemy_color = PURPLE;
            break;
        }
        case Enemy::Type::Green:
        {
            enemy_color = GREEN;
            break;
        }
        }

        // If enemy is hit, apply transparency and play sound
        if (enemy.get_hit_timer())
        {
            // Use semi-transparent white color to create a flash effect without disappearing
            // This creates a flashing effect by blending with white
            enemy_color = Color{255, 255, 255, 128};
            [[maybe_unused]] bool played = _enemydestroy.Play();
        }

        // Access the animation array directly using the enum's underlying value
        // Get the index directly without casting by using std::to_underlying (C++23) or direct cast
        // to int
        const auto enemyTypeIndex = static_cast<int>(enemy.get_type());
        const auto &animation = _enemy_animations[enemyTypeIndex];
        animation.draw(ds, enemy.get_x(), enemy.get_y(), enemy_color);

        // Per-enemy impact markers removed; global HitManager draws all hit decals.
    }
}

void EnemyManager::reset(GameTypes::Level i_level)
{
    // RESET EVERYTHING!!!!
    GameTypes::Count enemy_x = 0;
    GameTypes::Count enemy_y = 0;

    std::string level_sketch = "";
    _move_pause = std::max<short>(GlobalConstant::Int::ENEMY_MOVE_PAUSE_START_MIN,
                                  GlobalConstant::Int::ENEMY_MOVE_PAUSE_START -
                                      GlobalConstant::Int::ENEMY_MOVE_PAUSE_DECREASE * i_level);
    _move_timer = _move_pause;

    _shoot_distribution = std::uniform_int_distribution<GameTypes::Probability>(
        0, std::max<GameTypes::Probability>(GlobalConstant::Int::ENEMY_SHOOT_CHANCE_MIN,
                                            GlobalConstant::Int::ENEMY_SHOOT_CHANCE -
                                                GlobalConstant::Int::ENEMY_SHOOT_CHANCE_INCREASE *
                                                    i_level));

    for (Animation &enemy_animation : _enemy_animations)
    {
        enemy_animation.reset();
    }

    _enemy_bullets.clear();

    _enemies.clear();
    // There are 8 levels. Once the player finishes level 8, we go back to level 4. This is the same
    // thing we did in the game "Frogger".
    //  Fine-tune: Base hitbox is now 93.3% of BASE_WIDTH (based on sprite bitmap analysis),
    //  centered for precise collision detection.
    if (GlobalConstant::Int::TOTAL_LEVELS <= i_level)
    {
        // Since GlobalConstant::Int::TOTAL_LEVELS is 8 (a small number), we can avoid casts
        constexpr unsigned short HALF_LEVELS = GlobalConstant::Int::TOTAL_LEVELS / 2; // 4
        i_level = HALF_LEVELS + (i_level % HALF_LEVELS);
    }

    // Here you can see my pro level design skills!
    switch (i_level)
    {
    case 0:
    {
        level_sketch = "0 0 0 0 0 0 0 0 \n 0 0 0 0 0 0 0 0\n0 0 0 0 0 0 0 0 \n 0 0 0 0 0 0 0 0";

        break;
    }
    case 1:
    {
        level_sketch = "0000000000000000\n0000000000000000\n0000000000000000\n0000000000000000";

        break;
    }
    case 2:
    {
        // OH MY GOD, IS THAT A CHECKBOARD PATTERN?!
        level_sketch = "1010101010101010\n0101010101010101\n1010101010101010\n0101010101010101";

        break;
    }
    case 3:
    {
        level_sketch = "1111111111111111\n1111111111111111\n1111111111111111\n1111111111111111";

        break;
    }
    case 4:
    {
        // CHECKBOARD PATTERN WITH A SOLID PATTERN! I'M A GENIUS!
        level_sketch = "2222222222222222\n1111111111111111\n1010101010101010\n0101010101010101";

        break;
    }
    case 5:
    {
        // EASY ENEMIES AT THE TOP AND HARD ENEMIES AT THE BOTTOM! IT'S A REVOLUTION IN LEVEL
        // DESIGN!
        level_sketch = "0000000000000000\n2222222222222222\n1111111111111111\n1111111111111111";

        break;
    }
    case 6:
    {
        // CHECKBOARD PATTERN AGAIN?!
        level_sketch = "2121212121212121\n1212121212121212\n2121212121212121\n1212121212121212";

        break;
    }
    case 7:
    {
        level_sketch = "2222222222222222\n2222222222222222\n2222222222222222\n2222222222222222";
    }
    }

    // Here we're converting each character into an enemy.
    GameTypes::Health enemy_health = 1;
    for (const char sketch_character : level_sketch)
    {
        enemy_x++;

        switch (sketch_character)
        {
        case '\n':
        {
            enemy_x = 0;
            enemy_y++;

            break;
        }
        case '0':
        {
            const float enemyXPos =
                GlobalConstant::BASE_SIZE *
                (1.0f + enemy_x); // enemy_x is unsigned char, implicit conversion to float
            const float enemyYPos =
                GlobalConstant::BASE_SIZE *
                (2.0f + enemy_y); // enemy_y is unsigned char, implicit conversion to float
            _enemies.emplace_back(Enemy::Type::Cyan, enemyXPos, enemyYPos, enemy_health);
            break;
        }
        case '1':
        {
            const float enemyXPos = GlobalConstant::BASE_SIZE * (1.0f + enemy_x);
            const float enemyYPos = GlobalConstant::BASE_SIZE * (2.0f + enemy_y);
            _enemies.emplace_back(Enemy::Type::Purple, enemyXPos, enemyYPos, enemy_health);
            break;
        }
        case '2':
        {
            const float enemyXPos = GlobalConstant::BASE_SIZE * (1.0f + enemy_x);
            const float enemyYPos = GlobalConstant::BASE_SIZE * (2.0f + enemy_y);
            _enemies.emplace_back(Enemy::Type::Green, enemyXPos, enemyYPos, enemy_health);
        }
        }
    }
}

void EnemyManager::update(std::mt19937_64 &i_random_engine)
{
    std::vector<Enemy>::iterator dead_enemies_start;

    if (!_move_timer)
    {
        _move_timer = _move_pause;

        for (Enemy &enemy : _enemies)
        {
            enemy.move();
        }

        for (Animation &enemy_animation : _enemy_animations)
        {
            // The enemies change their frame after each move.
            enemy_animation.change_current_frame();
        }
        [[maybe_unused]] bool played = _enemymove.Play();
    }
    else
    {
        _move_timer--;
    }

    for (Enemy &enemy : _enemies)
    {
        enemy.update();

        if (!_shoot_distribution(i_random_engine))
        {
            enemy.shoot(_enemy_bullets);
        }
    }

    // I used a lambda!
    // I'm a pro!
    // No, not like that.
    // I'M A PROFESSIONAL C++ PROGRAMMER!!!!
    // Yeah, that's better.
    dead_enemies_start = remove_if(
        _enemies.begin(), _enemies.end(), [](const Enemy &i_enemy)
        { return 0 == i_enemy.get_health(); }); // The more enemies we kill, the faster they become.
    // No need for casting with int type
    auto alive_count = std::distance(dead_enemies_start, _enemies.end());
    // Specify explicit template parameter to help the compiler
    auto new_pause = std::max<int>(GlobalConstant::Int::ENEMY_MOVE_PAUSE_MIN,
                                   _move_pause - GlobalConstant::Int::ENEMY_MOVE_PAUSE_DECREASE *
                                                     static_cast<int>(alive_count));
    _move_pause = std::max<int>(0, new_pause);

    _enemies.erase(dead_enemies_start, _enemies.end());

    for (Bullet &enemy_bullet : _enemy_bullets)
    {
        enemy_bullet.update();
    }

    // I used a lambda!
    // AGAIN!
    _enemy_bullets.erase(remove_if(_enemy_bullets.begin(), _enemy_bullets.end(),
                                   [](const Bullet &i_bullet) { return i_bullet.IsDead(); }),
                         _enemy_bullets.end());
}

// Yes, that's a reference return type.
std::vector<Bullet> &EnemyManager::get_enemy_bullets() noexcept { return _enemy_bullets; }

std::vector<Enemy> &EnemyManager::get_enemies() noexcept { return _enemies; }
