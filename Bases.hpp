#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"
#include "Base.hpp"
#include "HitManager.hpp"

class Bases {
public:
    explicit Bases(const std::string& filename);
    ~Bases();
    Bases(const Bases&) = delete;
    auto operator=(const Bases &) -> Bases &     = delete;
    Bases(Bases&&) noexcept = default;
    auto operator=(Bases &&) noexcept -> Bases & = default;

    void reset();
    void update(std::vector<Bullet>& i_enemy_bullets, class HitManager& hits);
    void draw(raylib::DrawSession& ds) const;

private:
    std::vector<Base> _bases;
    // Change to int to avoid casting between size_t and unsigned short
    int _framecount = 0;
    Image _baseImage{}; // Store the original base image to reset bases
};