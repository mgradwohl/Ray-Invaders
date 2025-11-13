cmake_minimum_required(VERSION 3.20)
project(Ray_Invaders LANGUAGES CXX)

# If available, set CMP0169 to NEW so CMake won't warn about
# FetchContent_Populate being deprecated in favor of
# FetchContent_MakeAvailable when included third-party CMake code
# still calls the older API. This avoids noisy developer warnings.
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 NEW)
endif()

# Ensure modern find_package behavior for package root variables
# CMP0144 makes find_package consult the upper-case <PACKAGENAME>_ROOT
# variables (e.g. RAYLIB_ROOT) instead of the legacy lower-case names.
# Set it to NEW so we don't get developer warnings about ignoring RAYLIB_ROOT.
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

# Use CMake defaults for output layout per user preference.

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands to help clang tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(ENABLE_LTO "Enable LTO (where supported)" OFF)
option(ENABLE_NATIVE "Enable -march=native (where supported)" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks during build" OFF)
set(CLANG_TIDY_EXECUTABLE "clang-tidy-21" CACHE STRING "clang-tidy executable to use when ENABLE_CLANG_TIDY=ON")

message(STATUS "Configuring Ray-Invaders (C++${CMAKE_CXX_STANDARD})")


# Collect sources (simple wildcard at top level, exclude legacy xoshiro256plusplus sources)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cpp"
)


# Filter out any files that live under the binary dir (build/) or _deps/xoshiro-src (legacy, wrong repo)
set(_filtered_sources)
foreach(_src ${PROJECT_SOURCES})
    string(FIND "${_src}" "${CMAKE_BINARY_DIR}" _is_in_build_dir)
    string(FIND "${_src}" "CMakeFiles" _has_cmakefiles)
    string(FIND "${_src}" "CompilerId" _has_compilerid)
    string(FIND "${_src}" "CMakeCXXCompilerId.cpp" _has_compilerid_cpp)
    string(FIND "${_src}" "_deps/xoshiro-src" _is_legacy_xoshiro)
    if(_is_in_build_dir EQUAL -1 AND _has_cmakefiles EQUAL -1 AND _has_compilerid EQUAL -1 AND _has_compilerid_cpp EQUAL -1 AND _is_legacy_xoshiro EQUAL -1)
        list(APPEND _filtered_sources "${_src}")
    endif()
endforeach()

set(PROJECT_SOURCES ${_filtered_sources})





# --- XOSHIRO random number generator integration ---
# Try to find xoshiro (future-proofing for system install)
find_package(xoshiro CONFIG QUIET)

set(_xoshiro_header_dir "")
if (xoshiro_FOUND)
    message(STATUS "Found xoshiro via config package.")
    if (TARGET xoshiro::xoshiro)
        get_target_property(_xoshiro_header_dir xoshiro::xoshiro INTERFACE_INCLUDE_DIRECTORIES)
    endif()
else()
    include(FetchContent)
    FetchContent_Declare(
        xoshiro
        GIT_REPOSITORY https://github.com/nessan/xoshiro.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(xoshiro)
    set(_xoshiro_header_dir "${xoshiro_SOURCE_DIR}")
    message(STATUS "Fetched xoshiro from nessan/xoshiro: ${_xoshiro_header_dir}")
endif()

add_executable(Ray-Invaders ${PROJECT_SOURCES})
target_include_directories(Ray-Invaders PRIVATE ${CMAKE_SOURCE_DIR})
if(_xoshiro_header_dir)
    target_include_directories(Ray-Invaders PRIVATE "${_xoshiro_header_dir}/include")
endif()

# On Windows, force lld as the linker for all compilers (MSVC, Clang, etc.)
# This provides faster linking and better compatibility across toolchains.
# Do NOT explicitly re-link common system libs here; the raylib target (or the system) already
# declares the necessary system dependencies and duplicating them can lead
# to symbol replacement / duplicate issues with the linker.
if (WIN32)
    target_link_options(Ray-Invaders PRIVATE "-fuse-ld=lld")
endif()


# Try to find raylib first (package config)
find_package(raylib CONFIG QUIET)

set(_raylib_target_linked FALSE)
if (raylib_FOUND)
    if (TARGET raylib::raylib)
        message(STATUS "Found raylib via config package (target raylib::raylib)")
        target_link_libraries(Ray-Invaders PRIVATE raylib::raylib)
        set(_raylib_target_linked TRUE)
    elseif (TARGET raylib)
        message(STATUS "Found raylib via config package (target raylib)")
        target_link_libraries(Ray-Invaders PRIVATE raylib)
        set(_raylib_target_linked TRUE)
    else()
        message(WARNING "find_package(raylib) reported found but did not provide an imported target. Will fetch raylib.")
    endif()
endif()

if (NOT _raylib_target_linked)
    include(FetchContent)
    set(RAYLIB_GIT_TAG "5.5" CACHE STRING "Tag or branch of raylib to fetch when not found")
    message(STATUS "Acquiring raylib via FetchContent (GIT_TAG=${RAYLIB_GIT_TAG})")
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG ${RAYLIB_GIT_TAG}
    )
    FetchContent_MakeAvailable(raylib)

    

    if (TARGET raylib)
        message(STATUS "Using fetched raylib target")
        target_link_libraries(Ray-Invaders PRIVATE raylib)
    elseif (TARGET raylib::raylib)
        message(STATUS "Using fetched raylib::raylib target")
        target_link_libraries(Ray-Invaders PRIVATE raylib::raylib)
    else()
        message(WARNING "Fetched raylib but couldn't locate a usable target. You may need to install raylib manually.")
    endif()
endif()

# Clang-tidy support (opt-in)
if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_BIN ${CLANG_TIDY_EXECUTABLE})
    if (CLANG_TIDY_BIN)
        message(STATUS "Enabling clang-tidy: ${CLANG_TIDY_BIN}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_BIN}")
    else()
        message(WARNING "clang-tidy requested but ${CLANG_TIDY_EXECUTABLE} not found on PATH")
    endif()
endif()

# LTO / native hints
if (ENABLE_NATIVE)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_NATIVE)
    if (COMPILER_SUPPORTS_NATIVE)
        target_compile_options(Ray-Invaders PRIVATE "-march=native")
    endif()
endif()

if (ENABLE_LTO AND CMAKE_BUILD_TYPE STREQUAL "Release")
    # Direct LTO implementation that bypasses CMake's unreliable check_ipo_supported()
    # which often fails to detect LTO support for clang with lld linker.
    # Only enable LTO for Release builds to avoid debug performance issues.
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Enable LTO via compiler flags for Clang
        target_compile_options(Ray-Invaders PRIVATE -flto)
        target_link_options(Ray-Invaders PRIVATE -flto)
        message(STATUS "Enabled LTO via direct compiler flags for Clang (Release build)")
    elseif (MSVC)
        # Use CMake's built-in IPO for MSVC which is more reliable
        include(CheckIPOSupported)
        check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
        if (ipo_supported)
            set_target_properties(Ray-Invaders PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
            message(STATUS "Enabled LTO via CMake IPO for MSVC (Release build)")
        else()
            message(STATUS "LTO requested but not supported for MSVC: ${ipo_error}")
        endif()
    else()
        message(STATUS "LTO requested but compiler ${CMAKE_CXX_COMPILER_ID} not supported")
    endif()
elseif (ENABLE_LTO)
    message(STATUS "LTO requested but skipped for ${CMAKE_BUILD_TYPE} build (LTO only enabled for Release)")
endif()

# Explicit, unambiguous optimization level for Release/optimized builds.
# Some generators/toolchains set their own flags for Release, but we set
# the optimization level explicitly to ensure "optimized" presets use -O3
# on GCC/Clang and /O2 on MSVC. Use a generator-expression so the flag is
# applied only for the Release configuration.
if(MSVC)
    target_compile_options(Ray-Invaders PRIVATE $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(Ray-Invaders PRIVATE $<$<CONFIG:Release>:-O3>)
endif()

# Copy compile_commands.json to project root for clangd to find consistently
# This ensures clangd always uses the most recently configured preset's include paths
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_command(TARGET Ray-Invaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
        COMMENT "Copying compile_commands.json to project root for clangd"
    )
endif()

# Resources: copy Resources/ into the runtime directory after build so the exe can find them
add_custom_command(TARGET Ray-Invaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:Ray-Invaders>/Resources"
    COMMENT "Copying Resources/ to runtime output"
)

message(STATUS "Sources: ${PROJECT_SOURCES}")

# Convenience target so IDEs/tasks can run the game via: cmake --build --preset <p> --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:Ray-Invaders>
    DEPENDS Ray-Invaders
    COMMENT "Run Ray-Invaders from the build tree"
)
