#ifndef PLATFORM_H
#define PLATFORM_H

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#else
    #error "Unknown platform!"
#endif

// Vulkan extensions based on platform
#ifdef PLATFORM_MACOS
    #define ENABLE_PORTABILITY_EXTENSIONS 1
#else
    #define ENABLE_PORTABILITY_EXTENSIONS 0
#endif

#endif // PLATFORM_H
