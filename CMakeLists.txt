cmake_minimum_required(VERSION 3.12)
project(Lumen C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_COMPILER "") # Ensures C++ is not used

# Add the Vulkan library
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

# Add source files
add_executable(Lumen 
    src/main.c 
    #src/core/platform.c
    #src/core/logger.c
    #src/renderer/renderer.c
    src/renderer/vulkan/vulkan_renderer.c
)

# Include directories
target_include_directories(Lumen PRIVATE src inc)

# Link Vulkan
target_link_libraries(Lumen PRIVATE Vulkan::Vulkan glfw)