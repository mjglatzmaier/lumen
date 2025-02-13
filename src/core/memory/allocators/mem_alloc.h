#ifndef LUMEN_ALLOCATOR_H
#define LUMEN_ALLOCATOR_H

#include <stddef.h>  // for size_t

typedef struct lum_allocator {
    void* (*alloc)(struct lum_allocator* self, size_t size, size_t alignment);
    void  (*free)(struct lum_allocator* self, void* ptr);
    void* (*realloc)(struct lum_allocator* self, void* ptr, size_t new_size);
    void  (*reset)(struct lum_allocator* self);  // Optional (for arenas)
    void* user_data;  // For storing allocator-specific state
    
    // **New GPU allocation functions** (for Vulkan, DX12, CUDA)
    void* (*gpu_alloc)(struct lum_allocator* self, size_t size);
    void  (*gpu_free)(struct lum_allocator* self, void* ptr);
} lum_allocator;

// Default allocator (uses malloc/free)
extern lum_allocator* lum_get_default_allocator(void);

#endif // LUMEN_ALLOCATOR_H
