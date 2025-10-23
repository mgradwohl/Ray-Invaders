# Ray-Invaders Makefile
# Build configuration for Linux with Clang 21 and raylib

# Compiler and tools
CXX = clang++-21
LD = clang++-21

# Project settings
TARGET = ray-invaders
SRC_DIR = .
# Use separate build directories per configuration to avoid object mixing (e.g., LTO bitcode)
BUILD_ROOT = build

# C++ Standard and optimization flags
# We'll define per-config flags below
LDFLAGS_DEBUG =
LDFLAGS_RELEASE =
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Optional Release optimizations (toggle via: make release ENABLE_LTO=1 ENABLE_NATIVE=1)
ENABLE_LTO ?= 0
ENABLE_NATIVE ?= 0

# Include directories
INCLUDES = -I$(SRC_DIR) -I/usr/local/include

# Per-config build directories and object lists
BUILD_DIR_DEBUG = $(BUILD_ROOT)/Debug
BUILD_DIR_RELEASE = $(BUILD_ROOT)/Release

OBJECTS_DEBUG = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR_DEBUG)/%.o)
OBJECTS_RELEASE = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR_RELEASE)/%.o)

# Per-config compile flags
CXXFLAGS_DEBUG = -std=c++23 -Wall -Wextra -Wpedantic -g -O0
CXXFLAGS_RELEASE = -std=c++23 -Wall -Wextra -Wpedantic -O3 -DNDEBUG

# Find all source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Default target
.PHONY: all clean debug release run run-release install analyze fix profile perf-report callgrind help

all: debug

# Debug build (default)
debug: $(BUILD_DIR_DEBUG)/$(TARGET)

# Release build
ifeq ($(ENABLE_LTO),1)
CXXFLAGS_RELEASE += -flto=thin
LDFLAGS_RELEASE += -flto=thin
endif
ifeq ($(ENABLE_NATIVE),1)
CXXFLAGS_RELEASE += -march=native -mtune=native
endif
release: $(BUILD_DIR_RELEASE)/$(TARGET)

# Link the executables (per config)
$(BUILD_DIR_DEBUG)/$(TARGET): $(OBJECTS_DEBUG) | $(BUILD_DIR_DEBUG)
	@echo "Linking $(TARGET)..."
	$(LD) $(LDFLAGS_DEBUG) $^ -o $@ $(LIBS)
	@echo "Build complete: $@"

$(BUILD_DIR_RELEASE)/$(TARGET): $(OBJECTS_RELEASE) | $(BUILD_DIR_RELEASE)
	@echo "Linking $(TARGET)..."
	$(LD) $(LDFLAGS_RELEASE) $^ -o $@ $(LIBS)
	@echo "Build complete: $@"

# Compile source files (per config)
$(BUILD_DIR_DEBUG)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR_DEBUG)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

$(BUILD_DIR_RELEASE)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR_RELEASE)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS_RELEASE) $(INCLUDES) -c $< -o $@

# Create build directories
$(BUILD_DIR_DEBUG):
	mkdir -p $(BUILD_DIR_DEBUG)

$(BUILD_DIR_RELEASE):
	mkdir -p $(BUILD_DIR_RELEASE)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_ROOT)

# Run the game (defaults to Debug build)
run: $(BUILD_DIR_DEBUG)/$(TARGET)
	@echo "Running $(TARGET)..."
	cd $(SRC_DIR) && ./$(BUILD_DIR_DEBUG)/$(TARGET)

# Run the Release build
run-release: $(BUILD_DIR_RELEASE)/$(TARGET)
	@echo "Running $(TARGET) [Release]..."
	cd $(SRC_DIR) && ./$(BUILD_DIR_RELEASE)/$(TARGET)

# Profile with Linux perf (records until you quit the game)
profile: release
	@echo "Profiling with perf (Ctrl+C to stop)..."
	@command -v perf >/dev/null 2>&1 || { echo "Error: 'perf' is not installed.\nTry: sudo apt install linux-tools-common linux-tools-generic"; exit 1; }
	perf record -g --call-graph=dwarf -- $(BUILD_DIR_RELEASE)/$(TARGET)

# View perf report from last profile run
perf-report:
	perf report

# Optional: Callgrind profile (heavier, slower)
callgrind: release
	@echo "Running under callgrind (this will be slow)..."
	@command -v valgrind >/dev/null 2>&1 || { echo "Error: 'valgrind' is not installed.\nTry: sudo apt install valgrind"; exit 1; }
	valgrind --tool=callgrind -- $(BUILD_DIR_RELEASE)/$(TARGET)

# Install dependencies (if needed)
install:
	@echo "Installing dependencies..."
	sudo apt update
	sudo apt install -y clang-21 build-essential git cmake libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev

# Code analysis with clang-tidy
analyze:
	@echo "Running clang-tidy analysis..."
	./run-clang-tidy.sh

# Code analysis with automatic fixes
fix:
	@echo "Running clang-tidy with automatic fixes..."
	./run-clang-tidy.sh --fix

# Show help
help:
	@echo "Ray-Invaders Makefile"
	@echo "Available targets:"
	@echo "  all      - Build debug version (default)"
	@echo "  debug    - Build debug version with symbols"
	@echo "  release  - Build optimized release version"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run the game"
	@echo "  analyze  - Run clang-tidy code analysis"
	@echo "  fix      - Run clang-tidy with automatic fixes"
	@echo "  install  - Install build dependencies"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Toggles for release (pass as make variables):"
	@echo "  ENABLE_LTO=1     Enable ThinLTO (-flto=thin)"
	@echo "  ENABLE_NATIVE=1  Enable -march=native -mtune=native"

# Dependencies
-include $(OBJECTS_DEBUG:.o=.d)
-include $(OBJECTS_RELEASE:.o=.d)

# Generate dependency files (per config)
$(BUILD_DIR_DEBUG)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR_DEBUG)
	@$(CXX) $(CXXFLAGS_DEBUG) $(INCLUDES) -MM -MP -MT $(@:.d=.o) $< > $@

$(BUILD_DIR_RELEASE)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR_RELEASE)
	@$(CXX) $(CXXFLAGS_RELEASE) $(INCLUDES) -MM -MP -MT $(@:.d=.o) $< > $@