project(LumenCore C)

# Define Source Files
set(LUM_CORE_SRC
    # HASHING
    hash/metrohash64.c
    hash/murmurhash.c
    hash/xxhash.c
    # MATH
    math/math_hash.c
    math/math_vec3.c
    math/math_vec4.c
    math/math_mat3.c
    math/math_mat4.c
    math/math_fast.c
    math/math_rand.c
    # MEMORY
    memory/allocators/mem_general.c
    memory/allocators/mem_frame.c
    memory/allocators/mem_stack.c
    memory/allocators/mem_pool.c
    # JOB SYSTEM/WORKER THREADS
    threads/lum_thread.c
    # SCHEDULING
    schedulers/lum_scheduler.c
    # CONTAINERS
    containers/cont_da.c
    containers/cont_hm.c
)

# Create the LumenCore Library
add_library(LumenCore STATIC ${LUM_CORE_SRC})

# Enable mimalloc if the option is set
if(USE_MIMALLOC)
    message(STATUS "Mimalloc is enabled in src/CMakeLists.txt")
    target_link_libraries(LumenCore mimalloc-static)
    target_compile_definitions(LumenCore PUBLIC USE_MIMALLOC)
endif()

# Include Directories
target_include_directories(LumenCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/threads
    ${CMAKE_CURRENT_SOURCE_DIR}/schedulers
    # Submodules
    ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/libdivide
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
