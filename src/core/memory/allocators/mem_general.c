#include "mem_alloc.h"
#include <stdlib.h>
#include "math_bits.h"

#ifdef USE_MIMALLOC
#include <mimalloc.h>
#endif

static void* general_alloc(lum_allocator* self, size_t size, size_t alignment) {
    (void)self;  // Unused

    #ifdef USE_MIMALLOC
        return mi_malloc_aligned(size, alignment);
    #else
        size = lum_align_up(size, alignment);
        return aligned_alloc(alignment, size);
    #endif
}

static void general_free(lum_allocator* self, void* ptr) {
    (void)self;  // Unused
    #ifdef USE_MIMALLOC
        mi_free(ptr);
    #else
        free(ptr);
    #endif
}

static void* general_realloc(lum_allocator* self, void* ptr, size_t new_size) {
    (void)self;  // Unused
    #ifdef USE_MIMALLOC
        return mi_realloc(ptr, new_size);
    #else
        return realloc(ptr, new_size);
    #endif
}

static void general_reset(lum_allocator* self) {
    (void)self;  // Unused
    #ifdef USE_MIMALLOC
        mi_collect(true);  // Suggests aggressive cleanup in mimalloc
    #endif
}

// Default system allocator
static lum_allocator default_allocator = {
    .alloc = general_alloc,
    .free = general_free,
    .realloc = general_realloc,
    .reset = general_reset,
    .user_data = NULL
};

lum_allocator* lum_get_default_allocator(void) {
    return &default_allocator;
}
