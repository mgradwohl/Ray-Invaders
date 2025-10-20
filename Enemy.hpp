#pragma once
#include <vector>
#include "Bullet.hpp"
#include "Global.hpp"
#include <raylib.h>


class Enemy
{
public:
	enum class Direction : char { Left = -1, Down = 0, Right = 1 };
	enum class Type : GameTypes::Count { Cyan = 0, Purple = 1, Green = 2 };
	Enemy(Type i_type, float i_x, float i_y, GameTypes::Health health = 1) noexcept;

    [[nodiscard]] auto get_health() const noexcept -> GameTypes::Health;
    [[nodiscard]] auto get_hit_timer() const noexcept -> GameTypes::Timer;
    [[nodiscard]] auto get_type() const noexcept -> Type;
    [[nodiscard]] auto get_x() const noexcept -> float;
    [[nodiscard]] auto get_y() const noexcept -> float;

    void hit() noexcept;
	void move();
	void shoot(std::vector<Bullet>& i_enemy_bullets);
	void update() noexcept;

    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;

	// Public marker type so renderers can use it in signatures if needed
	struct ImpactMarker { float x; float y; int ttl; float radius; };

	// Record a transient visual impact marker relative to the enemy's position
	// Extended default TTL so the burn-off effect lasts longer
	void add_impact_marker(float rel_x, float rel_y, int ttl = 120, float radius = 2.5f) noexcept;

	// Access impact markers for rendering
	[[nodiscard]] const std::vector<ImpactMarker>& impact_markers() const noexcept { return _impact_markers; }

  private:
	Direction _direction{Direction::Down};
	GameTypes::Health _health{1};
	//The enemy will appear white for a few frames after being hit, so that the player knows about it.
	GameTypes::Timer _hit_timer{0};
	Type _type{Type::Cyan};
    float         _x{0.0F};
    float         _y{0.0F};
    Sound _enemylaser{};

	std::vector<ImpactMarker> _impact_markers{};
};