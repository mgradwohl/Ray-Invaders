#pragma once
#include <string>
#include <random>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Animation.hpp"
#include "PowerUpItem.hpp"

class Ufo
{
public:
	Ufo(std::mt19937_64& i_random_engine);

    auto check_bullet_collision(std::mt19937_64 &i_random_engine, const Rectangle &i_bullet_hitbox)
        -> bool;

    auto check_powerup_collision(const Rectangle &i_player_hitbox) noexcept -> unsigned char;
    void draw(raylib::DrawSession& ds);
	void reset(bool i_dead, std::mt19937_64& i_random_engine);
	void update(std::mt19937_64& i_random_engine);

    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;

  private:
	bool _dead{false};
	bool _dead_animation_over{false};
	unsigned short _timer{0};

    float _explosion_x{0.0F};
    float _x{0.0F};
    float _y{0.0F};

    std::uniform_int_distribution<unsigned short> _powerup_distribution;
	//The UFO can appear after 12 seconds. Or 16 seconds. Or 14.652 seconds. We don't know for sure.
	std::uniform_int_distribution<unsigned short> _timer_distribution;
    std::vector<Animation>                        _powerup_animations;
    //Yes, the UFO is responsible for the powerups too.
    std::vector<PowerUpItem> _powerups;

    Animation _animation;
	Animation _explosion;
	raylib::WaveSound _ufoappearsound;
};