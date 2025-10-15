#pragma once
#include <array>
#include <raylib.h>

class Bullet {
public:
    explicit Bullet(float i_step_x, float i_step_y, float i_x, float i_y) noexcept;
    Bullet(const Bullet&) = default;
    Bullet(Bullet&&) noexcept = default;
    auto operator=(const Bullet &) -> Bullet &     = default;
    auto operator=(Bullet &&) noexcept -> Bullet & = default;
    ~Bullet() = default;

    [[nodiscard]] auto IsDead() const noexcept -> bool;
    void IsDead(bool dead) noexcept;
    void update() noexcept;
    [[nodiscard]] auto get_hitbox() const noexcept -> Rectangle;
    [[nodiscard]] auto get_x() const noexcept -> float;
    [[nodiscard]] auto get_y() const noexcept -> float;
    [[nodiscard]] auto get_previous_x() const noexcept -> const std::array<float, 3> &;
    [[nodiscard]] auto get_previous_y() const noexcept -> const std::array<float, 3> &;

  private:
    std::array<float, 3> _previous_x{};
    std::array<float, 3> _previous_y{};
    float _x = 0;
    float _y = 0;
    bool _dead = false;
    float _real_x = 0;
    float _real_y = 0;
    float _step_x = 0;
    float _step_y = 0;
};
