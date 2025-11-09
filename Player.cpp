// Corresponding header
#include "Player.hpp"

// Standard library headers
#include "Random.hpp"

// Third-party headers
#include <raylib.h>

// Project headers
#include "Animation.hpp"
#include "Collision.hpp"
#include "Enemy.hpp"
#include "Global.hpp"
#include "HitManager.hpp"
#include "RLDrawSession.hpp"
#include "RLWaveSound.hpp"
#include "Ufo.hpp"

Player::Player()
    : _bullet_sprite("Resources/Images/PlayerBullet.png"), _player_sprite("Resources/Images/Player.png"),
      _playerlasersound("Resources/Sounds/Player Laser.wav"), _powerupsound("Resources/Sounds/Power Up.wav"),
      _playerdestroysound("Resources/Sounds/Player Destroy.wav"), _playershieldsound("Resources/Sounds/Player Hit Shield.wav"),
      _explosion(GlobalConstant::Int::EXPLOSION_ANIMATION_SPEED, GlobalConstant::Int::BASE_SIZE, "Resources/Images/Explosion.png")
{
    // Avoid frequent re-allocations during gameplay (typical max concurrent bullets is small)
    _bullets.reserve(64);
    reset();
}

// Lives API
auto Player::get_lives() const noexcept -> int
{
    return _lives;
}

void Player::set_lives(int lives) noexcept
{
    _lives = lives;
}

void Player::add_lives(int n) noexcept
{
    _lives += n;
}

auto Player::lose_life() noexcept -> int
{
    if (_lives > 0)
    {
        _lives--;
    }

    return _lives;
}

auto Player::get_dead() const noexcept -> bool
{
    // Player is considered dead (game-over) when lives are exhausted
    return _lives <= 0;
}

auto Player::get_dead_animation_over() const noexcept -> bool
{
    return _dead_animation_over;
}

auto Player::get_current_power() const noexcept -> GameTypes::Count
{
    return _current_power;
}

auto Player::get_power_timer() const noexcept -> GameTypes::Duration
{
    return _power_timer;
}

auto Player::get_y() const noexcept -> float
{
    return _y;
}

// Mark the current life as destroyed and start explosion animation.
void Player::die() noexcept
{
    _destroyed = true;
    _dead_animation_over = false;
    _explosion.reset();
}

void Player::draw(raylib::DrawSession &ds) const
{
    if (!_destroyed)
    {
        Vector2 dest{_x, _y};
        // Using float constants to avoid static_cast
        const Rectangle source{GlobalConstant::BASE_SIZE * _current_power,
                               0.0F, GlobalConstant::BASE_SIZE, GlobalConstant::BASE_SIZE};
        ds.DrawTexture(_player_sprite.get(), source, dest, GlobalColors::COL_WHITE);

        for (const Bullet &bullet : _bullets)
        {
            dest.x = bullet.get_x();
            dest.y = bullet.get_y();
            ds.DrawTexture(_bullet_sprite.get(), source, dest, GlobalColors::COL_WHITE);
        }

        if (!_shield_animation_over)
        {
            // Once we get hit while having a shield, the shield will be destroyed. We'll show a
            // blue explosion.
            _explosion.draw(ds, _x, _y, GlobalColors::PLAYER_SHIELD_EXPLOSION);
        }
    }
    else if (!_dead_animation_over)
    {
        _explosion.draw(ds, _x, _y, GlobalColors::EXPLOSION_ORANGE_RED);
    }
}

void Player::reset()
{
    _destroyed = false;
    _dead_animation_over = false;
    _shield_animation_over = true;

    _current_power = 0;
    _reload_timer = 0;
    _power_timer = 0;
    _x = GlobalConstant::HALF * (GlobalConstant::SCREEN_WIDTH - GlobalConstant::BASE_SIZE);
    _y = GlobalConstant::SCREEN_HEIGHT - (2.0F * GlobalConstant::BASE_SIZE);

    _bullets.clear();

    _explosion.reset();
}

void Player::update(std::vector<Bullet> &i_enemy_bullets, std::vector<Enemy> &i_enemies, Ufo &i_ufo,
                    HitManager &i_hits)
{
    if (!_destroyed)
    {
        if (IsKeyDown(KEY_LEFT))
        {
            if (4 == _current_power)
            {
                // Mirrored controls power-DOWN!
                _x = std::min(GlobalConstant::PLAYER_MOVE_SPEED + _x, GlobalConstant::SCREEN_WIDTH - 2.0f * GlobalConstant::BASE_SIZE);
            }
            else
            {
                _x = std::max(_x - GlobalConstant::PLAYER_MOVE_SPEED, GlobalConstant::BASE_SIZE);
            }
        }

        if (IsKeyDown(KEY_RIGHT))
        {
            if (4 == _current_power)
            {
                // Mirrored controls power-DOWN!
                _x = std::max(_x - GlobalConstant::PLAYER_MOVE_SPEED, GlobalConstant::BASE_SIZE);
            }
            else
            {
                _x = std::min(GlobalConstant::PLAYER_MOVE_SPEED + _x, GlobalConstant::SCREEN_WIDTH - 2.0f * GlobalConstant::BASE_SIZE);
            }
        }

        if (_reload_timer == 0U)
        {
            if (IsKeyPressed(KEY_Z))
            {
                [[maybe_unused]] bool played = _playerlasersound.Play();

                if (2 == _current_power)
                {
                    _reload_timer = GlobalConstant::Int::FAST_RELOAD_DURATION;
                }
                else
                {
                    _reload_timer = GlobalConstant::Int::RELOAD_DURATION;
                }

                _bullets.emplace_back(0.0F, -GlobalConstant::PLAYER_BULLET_SPEED, _x, _y);

                if (3 == _current_power)
                {
                    const float leftX = _x - (GlobalConstant::THREE_EIGHTHS * GlobalConstant::BASE_SIZE);
                    const float rightX = _x + (GlobalConstant::THREE_EIGHTHS * GlobalConstant::BASE_SIZE);
                    _bullets.emplace_back(0.0F, -GlobalConstant::PLAYER_BULLET_SPEED, leftX, _y);
                    _bullets.emplace_back(0.0F, -GlobalConstant::PLAYER_BULLET_SPEED, rightX, _y);
                }
            }
        }
        else
        {
            _reload_timer--;
        }

        for (Bullet &enemy_bullet : i_enemy_bullets)
        {
            if (AabbIntersect(get_hitbox(), enemy_bullet.get_hitbox()))
            {
                // Compute intersection center for a precise impact point
                const Rectangle pHB = get_hitbox();
                const Rectangle bHB = enemy_bullet.get_hitbox();
                const float inter_x1 = std::max(pHB.x, bHB.x);
                const float inter_y1 = std::max(pHB.y, bHB.y);
                const float inter_x2 = std::min(pHB.x + pHB.width, bHB.x + bHB.width);
                const float inter_y2 = std::min(pHB.y + pHB.height, bHB.y + bHB.height);
                const float inter_w = std::max(0.0F, inter_x2 - inter_x1);
                const float inter_h = std::max(0.0F, inter_y2 - inter_y1);
                const float impact_world_x = inter_x1 + (GlobalConstant::HALF * inter_w);
                const float impact_world_y = inter_y1 + (GlobalConstant::HALF * inter_h);

                if (1 == _current_power)
                {
                    _current_power = 0;
                    _shield_animation_over = false;
                    // Non-fatal player hit (shielded) - presets for radius and TTL
                    [[maybe_unused]] bool played = _playershieldsound.Play();
                    i_hits.add_hit(HitSubject::Player, HitOutcome::NonFatal, impact_world_x, impact_world_y);
                }
                else
                {
                    // Fatal player hit for this life: mark destroyed and play sound
                    _destroyed = true;
                    _dead_animation_over = false;
                    _explosion.reset();
                    [[maybe_unused]] bool played = _playerdestroysound.Play();
                    i_hits.add_hit(HitSubject::Player, HitOutcome::Destroyed, impact_world_x, impact_world_y);
                }

                enemy_bullet.IsDead(true);
                break;
            }
        }

        GameTypes::Count powerup_type = i_ufo.check_powerup_collision(get_hitbox());

        if (0 < powerup_type)
        {
            _current_power = powerup_type;

            _power_timer = GlobalConstant::Int::POWERUP_DURATION;
            [[maybe_unused]] bool played = _powerupsound.Play();
        }

        if (_power_timer == 0U)
        {
            _current_power = 0;
        }
        else
        {
            _power_timer--;
        }

        if (!_shield_animation_over)
        {
            _shield_animation_over = _explosion.update();
        }
    }
    else if (!_dead_animation_over)
    {
        _dead_animation_over = _explosion.update();
    }

    for (Bullet &bullet : _bullets)
    {
        bullet.update();

        if (!bullet.IsDead())
        {
            if (i_ufo.check_bullet_collision(bullet.get_hitbox()))
            {
                // Use UFO hitbox center as the impact point (explosion draws around
                // _explosion_x/_y)
                const Rectangle uHB = i_ufo.get_hitbox();
                const float impact_world_x = uHB.x + (GlobalConstant::HALF * uHB.width);
                const float impact_world_y = uHB.y + (GlobalConstant::HALF * uHB.height);
                i_hits.add_hit(HitSubject::Ufo, HitOutcome::Destroyed, impact_world_x, impact_world_y);
                bullet.IsDead(true);
            }
        }
    }

    for (Enemy &enemy : i_enemies)
    {
        for (Bullet &bullet : _bullets)
        {
            if (!bullet.IsDead() && 0 < enemy.get_health() && AabbIntersect(enemy.get_hitbox(), bullet.get_hitbox()))
            {
                bullet.IsDead(true);

                // Compute intersection center for a precise impact point (similar to Base)
                const Rectangle eHB = enemy.get_hitbox();
                const Rectangle bHB = bullet.get_hitbox();
                const float inter_x1 = std::max(eHB.x, bHB.x);
                const float inter_y1 = std::max(eHB.y, bHB.y);
                const float inter_x2 = std::min(eHB.x + eHB.width, bHB.x + bHB.width);
                const float inter_y2 = std::min(eHB.y + eHB.height, bHB.y + bHB.height);
                const float inter_w = std::max(0.0F, inter_x2 - inter_x1);
                const float inter_h = std::max(0.0F, inter_y2 - inter_y1);
                const float impact_world_x = inter_x1 + (GlobalConstant::HALF * inter_w);
                const float impact_world_y = inter_y1 + (GlobalConstant::HALF * inter_h);

                // Emit a global Enemy hit decal via HitManager (world-space). Presets for radius
                // and TTL
                i_hits.add_hit(HitSubject::Enemy, HitOutcome::NonFatal, impact_world_x, impact_world_y);

                enemy.hit();

                break;
            }
        }
    }

    _bullets.erase(remove_if(_bullets.begin(), _bullets.end(),
                             [](const Bullet &i_bullet)
                             {
                                 return i_bullet.IsDead();
                             }),
                   _bullets.end());
}

auto Player::get_hitbox() const noexcept -> Rectangle
{
    // Use the fraction constants from the F namespace
    return Rectangle{_x + (GlobalConstant::EIGHTH * GlobalConstant::BASE_SIZE),
                        _y + (GlobalConstant::EIGHTH * GlobalConstant::BASE_SIZE),
                     GlobalConstant::THREE_QUARTERS * GlobalConstant::BASE_SIZE,
                     GlobalConstant::THREE_QUARTERS * GlobalConstant::BASE_SIZE};
}

