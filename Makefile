# Ray-Invaders Makefile
# Build configuration for Linux with Clang 21 and raylib

# Compiler and tools
CXX = clang++-21
LD = clang++-21

# Project settings
TARGET = ray-invaders
SRC_DIR = .
BUILD_DIR = build
RESOURCES_DIR = Resources

# C++ Standard and optimization flags
CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -g -O0
LDFLAGS = 
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Include directories
INCLUDES = -I$(SRC_DIR) -I/usr/local/include

# Find all source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
.PHONY: all clean debug release run install

all: debug

# Debug build (default)
debug: CXXFLAGS += -DDEBUG
debug: $(BUILD_DIR)/$(TARGET)

# Release build
release: CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -O3 -DNDEBUG
release: $(BUILD_DIR)/$(TARGET)

# Link the executable
$(BUILD_DIR)/$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	@echo "Linking $(TARGET)..."
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo "Build complete: $@"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)

# Run the game
run: $(BUILD_DIR)/$(TARGET)
	@echo "Running $(TARGET)..."
	cd $(SRC_DIR) && ./$(BUILD_DIR)/$(TARGET)

# Install dependencies (if needed)
install:
	@echo "Installing dependencies..."
	sudo apt update
	sudo apt install -y clang-21 build-essential git cmake libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev

# Show help
help:
	@echo "Ray-Invaders Makefile"
	@echo "Available targets:"
	@echo "  all      - Build debug version (default)"
	@echo "  debug    - Build debug version with symbols"
	@echo "  release  - Build optimized release version"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run the game"
	@echo "  install  - Install build dependencies"
	@echo "  help     - Show this help message"

# Dependencies
-include $(OBJECTS:.o=.d)

# Generate dependency files
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@