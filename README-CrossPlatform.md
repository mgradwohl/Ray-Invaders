# Ray-Invaders - Cross-Platform Setup

This project supports both **Windows (MSVC)** and **Linux (Clang 21)** with C++23 and raylib.

## Platform-Specific Prerequisites

### Linux
- Clang 21 (already installed)
- raylib (installed from source)
- Standard build tools (make, cmake, git)
- X11 development libraries
- OpenGL libraries

### Windows
- Visual Studio 2022 with MSVC
- raylib installed to `c:\raylib\`
- Windows SDK

## Building the Project

### Linux - Using Make (Recommended)

```bash
# Debug build (default)
make debug

# Release build (optimized)  
make release

# Clean build artifacts
make clean

# Build and run
make run
```

### Windows - Using VS Code Tasks

Use the Windows-specific tasks:
- "C/C++: cl.exe build active file (Debug) - Windows"
- "C/C++: cl.exe build active file (Release) - Windows"

### Available Make Targets (Linux)

- `make` or `make debug` - Build debug version with symbols
- `make release` - Build optimized release version
- `make clean` - Remove all build artifacts
- `make run` - Build and run the game
- `make help` - Show available targets
- `make install` - Install build dependencies

## Using VS Code (Cross-Platform)

The project includes VS Code configuration files for both platforms:

### Linux Tasks
- **Build Debug (Linux - Makefile)** - Default Linux build
- **Build Release (Linux - Makefile)** - Release build  
- **Clean Build (Linux)** - Clean artifacts
- **Run Game (Linux)** - Build and run
- **C/C++: clang++-21 build active file (Direct) - Linux** - Direct compilation

### Windows Tasks
- **C/C++: cl.exe build active file (Debug) - Windows** - MSVC Debug build
- **C/C++: cl.exe build active file (Release) - Windows** - MSVC Release build

### Debug Configurations
- **(Linux) Launch Debug** - Debug on Linux with GDB
- **(Linux) Launch Release** - Release on Linux
- **(Windows) Launch Debug** - Debug on Windows with MSVC debugger  
- **(Windows) Launch Release** - Release on Windows

## Project Structure

```
Ray-Invaders/
├── build/                 # Build output directory
├── Resources/            # Game assets
│   ├── Images/          # Textures and sprites  
│   └── Sounds/          # Audio files
├── *.cpp, *.hpp         # Source code files
├── Makefile             # Build configuration
└── .vscode/             # VS Code configuration
    ├── tasks.json       # Build tasks
    ├── launch.json      # Debug configuration
    └── c_cpp_properties.json  # IntelliSense config
```

## Dependencies Installed

- **raylib 5.5.0** - Installed from source to `/usr/local/`
- **X11 libraries** - For window management
- **OpenGL libraries** - For graphics rendering
- **Audio libraries** - For sound support

## Notes

- The executable is built as `build/ray-invaders`
- All resource paths are relative to the project root
- Debug builds include symbols and debugging information
- Release builds are optimized for performance

## Troubleshooting

If you encounter build issues:

1. Ensure raylib is properly installed: `pkg-config --libs raylib`
2. Check Clang 21 installation: `clang++-21 --version`  
3. Verify all dependencies: `make install`
4. Clean and rebuild: `make clean && make debug`

For graphics issues, ensure you have proper OpenGL drivers installed.