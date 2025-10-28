// SO MANY CONSTANTS! AAAAAAAH!
#pragma once

#include <chrono>
#include <cstdint>

#include <raylib.h>

// Semantic type aliases for better code clarity and type safety
namespace GameTypes
{
// For small counts and sizes (0-255)
using Count = std::uint8_t;  // For counts of items, types, etc.
using Size = std::uint8_t;   // For pixel sizes, dimensions (within reason)
using Speed = std::uint8_t;  // For movement speeds, animation speeds
using Timer = std::uint8_t;  // For short-duration frame timers
using Health = std::uint8_t; // For health/hit points
using Level = std::uint8_t;  // For game levels

// For larger values (0-65535)
using Duration = std::uint16_t;    // For longer durations/timers
using Coordinate = std::uint16_t;  // For screen coordinates
using Distance = std::uint16_t;    // For larger distances/ranges
using Probability = std::uint16_t; // For probability values
} // namespace GameTypes

// All game constants consolidated into the GlobalConstant namespace
namespace GlobalConstant
{
// SECTION: Integer constants
namespace Int
{
// I didn't wanna make PLAYER_SIZE, ENEMY_SIZE, BULLET_SIZE, so I just defined the base size.
constexpr GameTypes::Size BASE_SIZE = 16;
constexpr GameTypes::Speed ENEMY_BULLET_SPEED = 2;
// I explained this in "Enemy.hpp". AND I DON'T LIKE TO REPEAT MYSELF!
constexpr GameTypes::Timer ENEMY_HIT_TIMER_DURATION = 2;
// The more enemies we kill, the faster they become.
constexpr GameTypes::Timer ENEMY_MOVE_PAUSE_DECREASE = 1;
// Setting this to 1 or even 0 results in terrible enemy movement. So don't do it.
//...
//...
// You're gonna do it anyway, aren't you?
constexpr GameTypes::Timer ENEMY_MOVE_PAUSE_MIN = 3;
//"Why not 64?" - you may ask. Well, it's because if we set it to 0, they will still have 1 frame
// pause. So 63 + 1 = 64.
constexpr GameTypes::Timer ENEMY_MOVE_PAUSE_START = 63;
constexpr GameTypes::Timer ENEMY_MOVE_PAUSE_START_MIN = 47;
constexpr GameTypes::Speed ENEMY_MOVE_SPEED = 2;
constexpr GameTypes::Count ENEMY_TYPES = 3;
constexpr GameTypes::Speed EXPLOSION_ANIMATION_SPEED = 2;
// Same logic with ENEMY_MOVE_PAUSE_START.
constexpr GameTypes::Timer FAST_RELOAD_DURATION = 7;
// The transition to the next level will last 64 frames or about 1 second.
constexpr GameTypes::Timer NEXT_LEVEL_TRANSITION = 64;
constexpr GameTypes::Speed PLAYER_BULLET_SPEED = 4;
constexpr GameTypes::Speed PLAYER_MOVE_SPEED = 2;
constexpr GameTypes::Speed POWERUP_ANIMATION_SPEED = 16;
constexpr GameTypes::Speed POWERUP_SPEED = 2;
constexpr GameTypes::Count POWERUP_TYPES = 4;
constexpr GameTypes::Timer RELOAD_DURATION = 31;
constexpr GameTypes::Size SCREEN_RESIZE = 5;
constexpr GameTypes::Level TOTAL_LEVELS = 8;
constexpr GameTypes::Speed UFO_ANIMATION_SPEED = 8;
constexpr GameTypes::Speed UFO_MOVE_SPEED = 1;
constexpr GameTypes::Count BASE_COUNT = 4;
constexpr GameTypes::Size BASE_WIDTH = 30;
    // Initial number of player lives at the start of a new game
    constexpr int INITIAL_LIVES = 3;
constexpr GameTypes::Size FONT_SIZE_BIG = 32;
// Reserved UI strip at top of window for banner (logical pixels)
constexpr GameTypes::Size BANNER_HEIGHT = 20;
// Padding for UI elements inside the banner (logical or screen pixels per usage)
constexpr GameTypes::Size BANNER_PADDING = 12;

// There are 64 enemies. So the probability that at least one of them shoots is 1 - (1 - 1 /
// 4096)^64 = 1.55% Keep in mind that we do this every frame.
constexpr GameTypes::Probability ENEMY_SHOOT_CHANCE = 4096;
constexpr GameTypes::Probability ENEMY_SHOOT_CHANCE_INCREASE = 64;
constexpr GameTypes::Probability ENEMY_SHOOT_CHANCE_MIN = 1024;
constexpr GameTypes::Duration POWERUP_DURATION = 512;
constexpr GameTypes::Coordinate SCREEN_HEIGHT = 180;
constexpr GameTypes::Coordinate WINDOW_HEIGHT = 200;
constexpr GameTypes::Coordinate SCREEN_WIDTH = 320;
// The UFO will appear after this number of frames.
constexpr GameTypes::Duration UFO_TIMER_MAX = 1024;
constexpr GameTypes::Duration UFO_TIMER_MIN = 768;
} // namespace Int

// SECTION: Time constants
namespace Time
{
constexpr std::chrono::microseconds FRAME_DURATION(16667); // ~60fps
}

// SECTION: Float constants to avoid casting operations
// Size constants
constexpr float BASE_SIZE = 16.0F;  // Int::GlobalConstant::BASE_SIZE
constexpr float BASE_WIDTH = 30.0F; // Int::BASE_WIDTH

// Speed constants
constexpr float ENEMY_BULLET_SPEED = 2.0F;  // Int::ENEMY_BULLET_SPEED
constexpr float ENEMY_MOVE_SPEED = 2.0F;    // Int::ENEMY_MOVE_SPEED
constexpr float PLAYER_BULLET_SPEED = 4.0F; // Int::PLAYER_BULLET_SPEED
constexpr float PLAYER_MOVE_SPEED = 2.0F;   // Int::PLAYER_MOVE_SPEED
constexpr float POWERUP_SPEED = 2.0F;       // Int::POWERUP_SPEED
constexpr float UFO_MOVE_SPEED = 1.0F;      // Int::GlobalConstant::UFO_MOVE_SPEED
constexpr float SCROLL_STEP = 1.0F;

// Screen dimensions
constexpr float SCREEN_HEIGHT = static_cast<float>(Int::SCREEN_HEIGHT);
constexpr float WINDOW_HEIGHT = static_cast<float>(Int::WINDOW_HEIGHT);
constexpr float SCREEN_WIDTH = static_cast<float>(Int::SCREEN_WIDTH);
constexpr float SCREEN_RESIZE = static_cast<float>(Int::SCREEN_RESIZE);
// Float mirrors for banner metrics
constexpr float BANNER_HEIGHT = static_cast<float>(Int::BANNER_HEIGHT);
constexpr float BANNER_PADDING = static_cast<float>(Int::BANNER_PADDING);

// Common position fractions (actually used in code)
constexpr float HALF = 0.5F;
constexpr float EIGHTH = 0.125F;
constexpr float THREE_EIGHTHS = 0.375F;
constexpr float QUARTER = 0.25F;
constexpr float THREE_QUARTERS = 0.75F;
} // namespace GlobalConstant

// Centralized color palette for custom colors used across modules
namespace GlobalColors
{
// Helper: return the given color with a different alpha
inline constexpr Color withAlpha(Color base, unsigned char alpha)
{
    return Color{base.r, base.g, base.b, alpha};
}

// Explosions and effects
inline constexpr Color PLAYER_SHIELD_EXPLOSION{0, 109, 255, 255};
inline constexpr Color EXPLOSION_ORANGE_RED{255, 36, 0, 255};

// Hit markers (core/outline)
inline constexpr Color HIT_BASE_CORE{200, 40, 40, 255};
inline constexpr Color HIT_BASE_OUTLINE{255, 120, 120, 255};

inline constexpr Color HIT_ENEMY_CORE{220, 60, 200, 255};
inline constexpr Color HIT_ENEMY_OUTLINE{255, 150, 230, 255};

inline constexpr Color HIT_PLAYER_DESTROYED_CORE{255, 140, 0, 255};
inline constexpr Color HIT_PLAYER_DESTROYED_OUTLINE{255, 200, 80, 255};

inline constexpr Color HIT_PLAYER_NONFATAL_CORE{40, 120, 255, 255};
inline constexpr Color HIT_PLAYER_NONFATAL_OUTLINE{120, 180, 255, 255};

inline constexpr Color HIT_UFO_CORE{60, 255, 180, 255};
inline constexpr Color HIT_UFO_OUTLINE{140, 255, 220, 255};

// Enemy base sprite colors
inline constexpr Color ENEMY_CYAN{0, 255, 255, 255};

// Aliases for raylib built-ins (centralized access) - prefixed to avoid macro name clashes
inline const Color COL_WHITE = ::WHITE;
inline const Color COL_BLACK = ::BLACK;
inline const Color COL_RED = ::RED;
inline const Color COL_ORANGE = ::ORANGE;
inline const Color COL_LIME = ::LIME;
inline const Color COL_GREEN = ::GREEN;
inline const Color COL_PURPLE = ::PURPLE;

// FPS meter colors
inline const Color FPS_GOOD = COL_LIME;
inline const Color FPS_WARN = COL_ORANGE;
inline const Color FPS_BAD = COL_RED;

// Enemy palette aliases for clarity
inline const Color ENEMY_PURPLE = COL_PURPLE;
inline const Color ENEMY_GREEN = COL_GREEN;

// Power-up UI bar colors
inline constexpr Color POWERUP_1{0, 219, 255, 255};
inline constexpr Color POWERUP_2{255, 109, 0, 255};
inline constexpr Color POWERUP_3{255, 219, 85, 255};
inline constexpr Color POWERUP_4{182, 109, 255, 255};
} // namespace GlobalColors
