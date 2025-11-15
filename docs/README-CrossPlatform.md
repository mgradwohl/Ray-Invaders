### Include ordering

For consistency across platforms and toolchains, follow this include order in source files:

1) Corresponding header first in .cpp (e.g., `#include "Foo.hpp"`)
2) Standard library headers (alphabetical)
3) Third-party headers (alphabetical) — e.g., `<raylib.h>`
4) Project headers (alphabetical)

Add a blank line between groups. In headers, only include `<raylib.h>` if your public API exposes raylib types; otherwise, include it in the `.cpp`.

See README.md for the canonical guideline.

# Ray-Invaders - Cross-Platform Setup

This project supports both Windows (MSVC) and Linux (Clang) using C++23 and raylib.

## Platform-Specific Prerequisites

### Linux
- Clang (system clang/clang++)
- CMake (>= 3.20) and Ninja
- raylib (system package or supplied via CMake FetchContent fallback)
- X11 / OpenGL development libraries
- Recommended editor tooling: clangd (language server), CodeLLDB (VS Code extension)

### Windows
- Visual Studio (MSVC)
- raylib installed to a Windows-accessible path (optional)

## Building the Project (Linux)

We use CMake presets with the Ninja generator. Presets are defined in `CMakePresets.json`.

Common workflows:

```
# Configure a build directory using the named preset (creates build/ninja-debug-clang etc.)
cmake --preset ninja-debug-clang

# Build using the same preset
cmake --build --preset ninja-debug-clang

# Run the built executable (output path depends on preset)
./build/ninja-debug-clang/Ray-Invaders

# Release/optimized presets
cmake --preset ninja-release
cmake --build --preset ninja-release

cmake --preset ninja-optimized
cmake --build --preset ninja-optimized
```

Notes:
- The `ninja-optimized` preset sets higher optimization and strips debug symbols for a smaller runtime binary.
- If raylib is not found on the system, CMake will fall back to fetching and building it automatically (see `CMakeLists.txt`).

## Using VS Code (Cross-Platform)

This repository provides VS Code configuration under `.vscode/`.

Linux workflow recommendations:
- Use the provided VS Code tasks which call CMake with presets (`tasks.json`).
- Debug using the CodeLLDB adapter (launch configurations use `type: "lldb"`).
- Use `clangd` for language services. The file `compile_commands.json` is automatically copied to the project root by CMake (see `CMakeLists.txt`), so no symlink is needed. You can also set `clangd.arguments` in `.vscode/settings.json` if you use a custom build directory.

Windows workflow:
- Existing Windows tasks and launch configurations are left intact for MSVC users.

## Project Structure

```
Ray-Invaders/
├── build/                 # Build output directories (created by CMake presets)
├── Resources/             # Game assets (Images/, Sounds/)
├── *.cpp, *.hpp           # Source code files
├── CMakeLists.txt         # CMake project definition
└── .vscode/               # VS Code configuration (tasks, launch, settings)
```

## Dependencies

- raylib — found via `find_package` / `pkg-config` or built via FetchContent fallback
- Clang, CMake, Ninja, X11/OpenGL dev libs

## Notes

- Built executables are located in `build/<preset>/Ray-Invaders`.
- Resource files are copied into the corresponding build output directory at configure/build time so the game can find them at runtime.
- Debug builds include symbols; optimized builds may be stripped depending on the preset.

## Troubleshooting

1. Check that a preset configures correctly: `cmake --preset ninja-debug-clang`.
2. Verify raylib availability: `pkg-config --libs raylib` or rely on CMake FetchContent.
3. If editor diagnostics look wrong, ensure `compile_commands.json` points to the active build directory (the workspace root symlink is convenient).
4. For debugging in VS Code, install the CodeLLDB extension and use the provided "(Linux) Launch Debug" configuration.

If you'd like, I can add a short developer setup section to the main README and optionally add a CI workflow that validates the Linux presets automatically.