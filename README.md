## Include ordering guideline

To keep headers tidy and reduce unnecessary rebuilds, follow this include order in all `.hpp` and `.cpp` files:

1. Corresponding header (only in `.cpp`): `#include "ThisFile.hpp"`
2. Standard library headers (alphabetical)
3. Third-party library headers (alphabetical) — e.g. `<raylib.h>`
4. Project headers (alphabetical)

Prefer a blank line between each group, and avoid including headers you don't need. In headers, only include `<raylib.h>` if the public interface uses raylib types (e.g., `Color`, `Rectangle`, `Vector2`); otherwise, prefer including it in the `.cpp`.

When adding new files, please follow this order to keep diffs minimal and builds fast.

## Coding conventions: constants and colors

To keep gameplay math and visuals consistent and easy to tweak, use these shared conventions:

- Shared numeric fractions live in `GlobalConstant` (see `Global.hpp`). Use these instead of repeating literals:
	- `GlobalConstant::HALF` (0.5F), `QUARTER` (0.25F), `THREE_QUARTERS` (0.75F)
	- `EIGHTH` (0.125F), `THREE_EIGHTHS` (0.375F)
- Tuning dials specific to one subsystem should be `constexpr` in that module’s `.cpp` (module‑local), not global.

Colors are centralized in `GlobalColors` (also in `Global.hpp`) so the palette is consistent across the game.

- Prefer named palette entries instead of ad‑hoc `Color{r,g,b,a}`:
	- Explosion/Hit: `EXPLOSION_CORE`, `EXPLOSION_GLOW`, `HIT_OUTLINE`
	- Power‑ups: `POWERUP_SPEED`, `POWERUP_SHIELD`, `POWERUP_TRIPLE`, `POWERUP_PIERCE`
	- Enemies/UI: `ENEMY_PURPLE`, `ENEMY_GREEN`, `ENEMY_CYAN`, `FPS_GOOD`, `FPS_WARN`, `FPS_BAD`
- Safe aliases for common raylib colors are provided to avoid platform differences: `COL_WHITE`, `COL_BLACK`, `COL_RED`, `COL_GREEN`, `COL_BLUE`, etc.
- Use `GlobalColors::withAlpha(color, a)` to apply transparency in a readable way.

Examples

```cpp
#include "Global.hpp"

// Fractions
float midX = width * GlobalConstant::HALF;

// Colors
DrawText("Score", 8, 8, 20, GlobalColors::COL_WHITE);

// Tinting with alpha (e.g., overlay)
DrawRectangle(0, 0, screenW, screenH, GlobalColors::withAlpha(GlobalColors::COL_BLACK, 128));

// Enemy palette usage
DrawCircleV(pos, 6.0F, GlobalColors::ENEMY_CYAN);
```

When introducing a new shared color, add it to `GlobalColors` with a descriptive name. If the color is a temporary tuning knob local to one effect, prefer a module‑local `constexpr Color` in that `.cpp`.

# Ray Invaders

This is a port of kofybrek's SFML Space Invaders to raylib. I'll also make it more object oriented and do some performance profiling and clean up the code.

To get this working:
1. Install raylib from raylib.com to the default directory
2. Open the raylib VS2022 project and build the release and debug libs for your platform (mine is x64)
3. close the raylib project
4. Sync the Ray-Invaders repro
5. Open the sln, build and have fun
