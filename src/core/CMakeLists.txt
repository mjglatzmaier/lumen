project(LumenCore C)

# Enable SIMD Support
option(USE_SIMD "Enable SIMD optimizations" ON)

# Define Source Files
set(LUM_CORE_SRC
    math/math_vec3.c
    math/math_vec4.c
    math/math_mat3.c
    math/math_mat4.c
    math/math_fast.c
)

# Create the LumenCore Library
add_library(LumenCore STATIC ${LUM_CORE_SRC})

# Include Directories
target_include_directories(LumenCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/simde
)

# Compiler Flags for SIMD
if(USE_SIMD)
    target_compile_definitions(LumenCore PUBLIC USE_SIMD)
    message(STATUS "SIMD support enabled")

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        target_compile_options(LumenCore PRIVATE -msse4.1 -mavx)
        message(STATUS "Using x86 SIMD (SSE4.1, AVX)")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
        target_compile_options(LumenCore PRIVATE -mfpu=neon)
        message(STATUS "Using ARM SIMD (NEON)")
    endif()
else()
    message(STATUS "SIMD support disabled")
endif()
