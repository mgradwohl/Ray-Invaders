#pragma once
#include <atomic>
#include <string>

#include <raylib.h>

#include "RLTexture2D.hpp"
#include "RLDrawSession.hpp"

class Popup
{
  public:
    Popup() = default;
    Popup(const Popup &) = delete;
    Popup(Popup &&) = delete;
    auto operator=(const Popup &) -> Popup & = delete;
    auto operator=(Popup &&) -> Popup & = delete;

    ~Popup() = default;

    // Load a graphic for the popup. Returns true on success.
    auto loadFromFile(const std::string &path) -> bool;

    // Show the popup.
    // holdFrames: how many frames to hold at full opacity (0 = until dismissed if waitForInput)
    // blockGame: whether this popup should block gameplay updates
    // waitForInput: if true, popup stays until Enter is pressed
    void show(int holdFrames, bool blockGame = true, bool waitForInput = false) noexcept;

    void hide() noexcept;

    // void setOnClose(std::function<void()> cb) noexcept
    // {
    //     _onClose = std::move(cb);
    // }

    void update() noexcept;
    void draw(raylib::DrawSession &ds) const noexcept;

    [[nodiscard]] auto isActive() const noexcept -> bool
    {
        return _state != State::Hidden;
    }

    [[nodiscard]] auto isBlocking() const noexcept -> bool
    {
        return _blocking;
    }

  private:
    enum class State : std::uint8_t
    {
        Hidden,
        // FadingIn,
        Holding,
        // FadingOut,
    };

    raylib::Texture2DFile _tex;

    State _state{State::Hidden};
    int _timer{0};
    int _holdFrames{0};
    // int _fadeIn{12};
    // int _fadeOut{12};
    bool _blocking{true};
    bool _waitForInput{false};
    float _alpha{0.0F};
    // std::function<void()> _onClose;
};
