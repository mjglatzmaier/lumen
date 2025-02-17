cmake_minimum_required(VERSION 3.12)
project(Lumen C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Detect build type (defaults to Debug if not specified)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Enable additional optimizations in Release mode
message(STATUS "Configuring build type: ${CMAKE_BUILD_TYPE}")

# Configuration-specific compiler options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Applying Debug compilation flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g -Wall -Wextra -Wpedantic -msse4.2 -mpopcnt -mlzcnt -mtune=native -DDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Applying Release compilation flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -msse4.2 -mpopcnt -mlzcnt -mtune=native -DNDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Applying RelWithDebInfo compilation flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -march=native -msse4.2 -mpopcnt -mlzcnt -mtune=native -DNDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(STATUS "Applying MinSizeRel compilation flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -msse4.2 -mpopcnt -mlzcnt -mtune=native -DNDEBUG")
endif()

# Check for SIMD support flag
option(USE_SIMD "Enable SIMD optimizations" OFF)

# Check for mimalloc support
option(USE_MIMALLOC "Enable mimalloc as the memory allocator" OFF)

if(USE_SIMD)
    message(STATUS "Enabling SIMD optimizations")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1 -mfma -mavx2 -DUSE_SIMD")
endif()

# Ensure a valid C++ compiler exists (needed for mimalloc)
if(NOT CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER NAMES clang++ g++ c++ REQUIRED)
endif()

# Add the Vulkan library
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

# Add source files
add_executable(Lumen 
    src/main.c 
    #src/core/platform.c
    #src/core/logger.c
    #src/renderer/renderer.c
    src/input/input.c
    src/renderer/vulkan/vulkan_renderer.c
)

# Add mimalloc if requested
if(USE_MIMALLOC)
    # Ensure mimalloc is built as a static library in C mode
    set(MI_BUILD_TESTS OFF CACHE BOOL "Disable mimalloc tests")
    set(MI_BUILD_SHARED OFF CACHE BOOL "Build mimalloc as a static library")
    set(MI_BUILD_STATIC ON CACHE BOOL "Ensure mimalloc is built as a static library")
    set(MI_OVERRIDE OFF CACHE BOOL "Disable C++ new/delete overrides")
    add_subdirectory(submodules/mimalloc)
    message(STATUS "Using mimalloc as allocator")
endif()

# Add the core module
add_subdirectory(src/core)

# Link
target_link_libraries(Lumen PRIVATE Vulkan::Vulkan glfw LumenCore m)

# Include directories
target_include_directories(Lumen PRIVATE
    src
    inc
    submodules/stb
    #submodules/imgui
    #submodules/FastNoiseLite/C
)

# Conditionally includes
if(USE_MIMALLOC)
    list(APPEND INCLUDE_DIRS submodules/mimalloc/include)
endif()

# Create unit test executable
add_executable(LumenTests
    tests/runner.c
    tests/math/math_mat_tests.c
    tests/math/math_vec_tests.c
    tests/memory/test_alloc_benchmark.c
    tests/memory/test_alloc_general.c
    tests/memory/test_alloc_frame.c
    tests/memory/test_alloc_stack.c
    tests/memory/test_alloc_pool.c
    tests/scheduling/test_lum_scheduler.c
    tests/containers/test_cont_da.c
    tests/containers/test_cont_lfq.c
    tests/containers/test_cont_lfq_mt.c)

target_link_libraries(LumenTests PRIVATE LumenCore m)

target_include_directories(LumenTests PRIVATE
    src
    inc
    submodules/stb
    #submodules/imgui
    #submodules/FastNoiseLite/C
)