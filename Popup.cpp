#include "Popup.hpp"

#include <cmath>
#include <string>

#include <raylib.h>

#include "RLDrawSession.hpp"

bool Popup::loadFromFile(const std::string &path)
{
    _tex.load(path);
    if (!IsTextureValid(_tex.get()))
    {
        _tex.unload();
        return false;
    }

    const int tid = _tex.id();
    return tid > 0;
}

void Popup::show(int holdFrames, bool blockGame, bool waitForInput) noexcept
{
    if (isActive())
    {
        return; // already visible
    }

    _holdFrames = holdFrames;
    _blocking = blockGame;
    _waitForInput = waitForInput;
    _state = State::Holding;
    _timer = 0;
    _alpha = 1.0F;
}

void Popup::hide() noexcept
{
    _state = State::Hidden;
    _timer = 0;
    _alpha = 0.0F;
    // if (_onClose)
    // {
    //     _onClose();
    // }
}

void Popup::update() noexcept
{
    switch (_state)
    {
    case State::Hidden:
        return;
    // case State::FadingIn:
    // {
    //     _timer++;
    //     const float t = static_cast<float>(_timer) / static_cast<float>(std::max(1, _fadeIn));
    //     _alpha = std::min(1.0F, t);
    //     if (_timer >= _fadeIn)
    //     {
    //         if (_holdFrames > 0 || !_waitForInput)
    //         {
    //             _state = State::Holding;
    //             _timer = 0;
    //         }
    //         else
    //         {
    //             // If waiting for input and no hold, remain visible until input.
    //             _state = State::Holding;
    //             _timer = 0;
    //         }
    //     }
    //     break;
    // }
    case State::Holding:
    {
        _alpha = 1.0F;
        if (_waitForInput)
        {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            {
                // _state = State::FadingOut;
                _timer = 0;
            }
        }
        else
        {
            _timer++;
            if (_timer >= _holdFrames)
            {
                // _state = State::FadingOut;
                _timer = 0;
            }
        }
        break;
    }
    // case State::FadingOut:
    // {
    //     _timer++;
    //     const float t = 1.0F - static_cast<float>(_timer) / static_cast<float>(std::max(1, _fadeOut));
    //     _alpha = std::max(0.0F, t);
    //     if (_timer >= _fadeOut)
    //     {
    //         _state = State::Hidden;
    //         _timer = 0;
    //         if (_onClose)
    //         {
    //             _onClose();
    //         }
    //     }
    //     break;
    // }
    }
}

void Popup::draw(raylib::DrawSession &ds) const noexcept
{
    if (!isActive())
    {
        return;
    }

    const Texture2D &tex = _tex.get();
    const float screenW = static_cast<float>(GlobalConstant::Int::SCREEN_WIDTH);
    const float screenH = static_cast<float>(GlobalConstant::Int::SCREEN_HEIGHT);
    const float scale = 0.5F;

    if (tex.id > 0)
    {
        // Debug: indicate we're drawing the popup and current alpha
        const float srcW = static_cast<float>(tex.width);
        const float srcH = static_cast<float>(tex.height);

        const float dstW = std::round(srcW * scale);
        const float dstH = std::round(srcH * scale);

        const float cx = (screenW - dstW) * 0.5F;
        const float cy = (screenH - dstH) * 0.5F;

        ds.DrawTextureEx(_tex.get(), cx, cy, 0.5F, GlobalColors::COL_WHITE);
    }
}
