#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"
#include "Global.hpp"
#include "Bullet.hpp"
#include "Base.hpp"

class Bases {
public:
    explicit Bases(const std::string& filename);
    ~Bases();
    Bases(const Bases&) = delete;
    Bases& operator=(const Bases&) = delete;
    Bases(Bases&&) noexcept = default;
    Bases& operator=(Bases&&) noexcept = default;

    void reset();
    void update(std::vector<Bullet>& i_enemy_bullets);
    void draw(raylib::DrawSession& ds) const;

private:
    std::vector<Base> _bases;
    std::size_t _framecount = 0;
    Texture2D _sprite;
};