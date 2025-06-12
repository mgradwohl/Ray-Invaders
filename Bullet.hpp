#pragma once
#include <array>
#include <raylib.h>

class Bullet {
public:
    explicit Bullet(float i_step_x, float i_step_y, float i_x, float i_y) noexcept;
    Bullet(const Bullet&) = default;
    Bullet(Bullet&&) noexcept = default;
    Bullet& operator=(const Bullet&) = default;
    Bullet& operator=(Bullet&&) noexcept = default;
    ~Bullet() = default;

    [[nodiscard]] bool IsDead() const noexcept;
    void IsDead(bool dead) noexcept;
    void update() noexcept;
    [[nodiscard]] Rectangle get_hitbox() const noexcept;
    [[nodiscard]] float get_x() const noexcept;
    [[nodiscard]] float get_y() const noexcept;
    [[nodiscard]] const std::array<float, 3>& get_previous_x() const noexcept;
    [[nodiscard]] const std::array<float, 3>& get_previous_y() const noexcept;

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
