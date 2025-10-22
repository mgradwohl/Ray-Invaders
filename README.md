## Include ordering guideline

To keep headers tidy and reduce unnecessary rebuilds, follow this include order in all `.hpp` and `.cpp` files:

1. Corresponding header (only in `.cpp`): `#include "ThisFile.hpp"`
2. Standard library headers (alphabetical)
3. Third-party library headers (alphabetical) — e.g. `<raylib.h>`
4. Project headers (alphabetical)

Prefer a blank line between each group, and avoid including headers you don't need. In headers, only include `<raylib.h>` if the public interface uses raylib types (e.g., `Color`, `Rectangle`, `Vector2`); otherwise, prefer including it in the `.cpp`.

When adding new files, please follow this order to keep diffs minimal and builds fast.

# Ray Invaders

This is a port of kofybrek's SFML Space Invaders to raylib. I'll also make it more object oriented and do some performance profiling and clean up the code.

To get this working:
1. Install raylib from raylib.com to the default directory
2. Open the raylib VS2022 project and build the release and debug libs for your platform (mine is x64)
3. close the raylib project
4. Sync the Ray-Invaders repro
5. Open the sln, build and have fun
