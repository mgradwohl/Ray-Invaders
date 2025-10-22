#pragma once
#include "Player.hpp"
#include "RLDrawSession.h"
#include "RLTexture2D.hpp"

class PowerUpManager
{
  public:
    PowerUpManager(const std::string &spritefile);
    ~PowerUpManager();
    void update(const Player &player);
    void draw(raylib::DrawSession &ds, const Player &player, float bannerHeightLogical) const;

    // Accessors for banner composition
    [[nodiscard]] auto get_sprite() const noexcept -> const Texture2D &
    {
        return _powerup_bar_sprite.get();
    }
    [[nodiscard]] auto get_color() const noexcept -> Color { return _color; }
    [[nodiscard]] auto get_fill_fraction(const Player &player) const noexcept -> float;

  private:
    raylib::Texture2DFile _powerup_bar_sprite;
    Color _color = WHITE;
    const std::string _spritefile;
};
