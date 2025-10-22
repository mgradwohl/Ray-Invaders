#pragma once
#include <memory>
#include <string>
#include <vector>

#include <raylib.h>

#include "Base.hpp"
#include "Bullet.hpp"
#include "Global.hpp"
#include "HitManager.hpp"
#include "RLDrawSession.h"
#include "RLWaveSound.hpp"

class Bases
{
  public:
    explicit Bases(const std::string &filename);
    ~Bases();
    Bases(const Bases &) = delete;
    auto operator=(const Bases &) -> Bases & = delete;
    Bases(Bases &&) noexcept = default;
    auto operator=(Bases &&) noexcept -> Bases & = default;

    void reset();
    void update(std::vector<Bullet> &i_enemy_bullets, class HitManager &hits);
    void draw(raylib::DrawSession &ds) const;

  private:
    std::vector<std::unique_ptr<Base>> _bases;
    // Change to int to avoid casting between size_t and unsigned short
    int _framecount = 0;
    Image _baseImage{}; // Store the original base image to reset bases
};