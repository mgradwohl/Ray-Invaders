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

# Collect sources (simple wildcard at top level)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cpp"
)

# Filter out any files that live under the binary dir (build/) because
# we run in-tree builds (build/ inside the source tree) for some presets
# and CMake will generate helper sources there (CompilerIdCXX). Those
# generated files must not be compiled into the main target.
set(_filtered_sources)
foreach(_src ${PROJECT_SOURCES})
    # If the source path contains the binary dir path, skip it
    string(FIND "${_src}" "${CMAKE_BINARY_DIR}" _is_in_build_dir)
    # Also skip any CompilerId / CMakeFiles helper files that might live in
    # other build directories (e.g. build/ninja-debug-clang/...). Those files
    # declare a 'main' symbol and will cause duplicate-main link errors.
    string(FIND "${_src}" "CMakeFiles" _has_cmakefiles)
    string(FIND "${_src}" "CompilerId" _has_compilerid)
    string(FIND "${_src}" "CMakeCXXCompilerId.cpp" _has_compilerid_cpp)
    if(_is_in_build_dir EQUAL -1 AND _has_cmakefiles EQUAL -1 AND _has_compilerid EQUAL -1 AND _has_compilerid_cpp EQUAL -1)
        list(APPEND _filtered_sources "${_src}")
    endif()
endforeach()

set(PROJECT_SOURCES ${_filtered_sources})

add_executable(Ray-Invaders ${PROJECT_SOURCES})

target_include_directories(Ray-Invaders PRIVATE ${CMAKE_SOURCE_DIR})

# On Windows, prefer lld-link when using LLVM's clang (matches the project's
# historical build script which passed -fuse-ld=lld-link). Do NOT explicitly
# re-link common system libs here; the raylib target (or the system) already
# declares the necessary system dependencies and duplicating them can lead
# to symbol replacement / duplicate issues with the linker.
if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        target_compile_options(Ray-Invaders PRIVATE "-fuse-ld=lld-link")
    endif()
endif()

# Try to find raylib first (package config), then fallback to pkg-config, then allow user override
find_package(raylib CONFIG QUIET)
if (raylib_FOUND)
    message(STATUS "Found raylib via config package")
    target_link_libraries(Ray-Invaders PRIVATE raylib::raylib)
else()
    find_package(PkgConfig QUIET)
    if (PkgConfig_FOUND)
        pkg_check_modules(RAYLIB pkg-config QUIET raylib)
    endif()
    if (RAYLIB_FOUND)
        message(STATUS "Found raylib via pkg-config")
        target_include_directories(Ray-Invaders PRIVATE ${RAYLIB_INCLUDE_DIRS})
        target_link_libraries(Ray-Invaders PRIVATE ${RAYLIB_LIBRARIES})
    else()
        # Fallback: allow user to set RAYLIB_ROOT or RAYLIB_DIR
        if (NOT DEFINED RAYLIB_ROOT)
            set(RAYLIB_ROOT "C:/raylib" CACHE PATH "Path to raylib root (fallback)")
        endif()
        # Try to find headers and a library under the user-provided root
        find_path(RAYLIB_INCLUDE_DIR raylib.h HINTS ${RAYLIB_ROOT} ${RAYLIB_ROOT}/include)
        find_library(RAYLIB_LIBRARY NAMES raylib raupinderaylib HINTS ${RAYLIB_ROOT} ${RAYLIB_ROOT}/lib)

        # Track whether we successfully found and will use a compatible prebuilt raylib
        set(_raylib_used FALSE)
        if (RAYLIB_INCLUDE_DIR AND RAYLIB_LIBRARY)
            # Detect common incompatibility: a MinGW-built static library (.a) being used with
            # MSVC/clang-cl + lld-link on Windows. That will produce unresolved symbols at link time.
            # Use a regex match for ".a" suffix to be robust.
            string(REGEX MATCH "\\.a$" _raylib_is_mingw_lib "${RAYLIB_LIBRARY}")
            if (WIN32 AND _raylib_is_mingw_lib AND (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
                message(WARNING "Found raylib at ${RAYLIB_ROOT} but it appears to be a MinGW static library (.a) which is incompatible with the current compiler/linker. Falling back to building raylib from source.")
            else()
                message(STATUS "Found raylib via RAYLIB_ROOT: ${RAYLIB_ROOT}")
                target_include_directories(Ray-Invaders PRIVATE ${RAYLIB_INCLUDE_DIR})
                target_link_libraries(Ray-Invaders PRIVATE ${RAYLIB_LIBRARY})
                set(_raylib_used TRUE)
            endif()
        endif()

        if (NOT _raylib_used)
            message(STATUS "raylib not found locally or not compatible; falling back to FetchContent to acquire raylib")
            include(FetchContent)
            # Overwrite any existing cached value to a robust default (use FORCE to replace stale cached tags)
            set(RAYLIB_GIT_TAG "master" CACHE STRING "Tag or branch of raylib to fetch when not found" FORCE)
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
        # If we ended up using a prebuilt RAYLIB_LIBRARY on Windows, ensure we link
        # common audio/system libs that may be required (e.g., timeBeginPeriod/timeEndPeriod live in winmm).
        if (WIN32 AND _raylib_used)
            target_link_libraries(Ray-Invaders PRIVATE winmm)
        endif()
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

if (ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if (ipo_supported)
        set_target_properties(Ray-Invaders PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO requested but not supported: ${ipo_error}")
    endif()
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
