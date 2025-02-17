#include "math_bits.h"
#include "mem_alloc.h"

#include <stdlib.h>
#include <string.h>

#ifdef USE_MIMALLOC
#include <mimalloc.h>
#endif

static void *general_alloc(lum_allocator *self, size_t size, size_t alignment)
{
    (void) self;
#ifdef USE_MIMALLOC
    return mi_malloc_aligned(size, alignment);
#else
    size = lum_align_up(size, alignment);
    return aligned_alloc(alignment, size);
#endif
}

static void general_free(lum_allocator *self, void *ptr)
{
    (void) self;
#ifdef USE_MIMALLOC
    mi_free(ptr);
#else
    free(ptr);
#endif
}

static void *general_realloc(lum_allocator *self, void *ptr, size_t new_size, size_t alignment)
{
    (void) self;
#ifdef USE_MIMALLOC
    return mi_realloc_aligned(ptr, new_size, alignment);
#else
    // Ensure the new size is aligned
    new_size = lum_align_up(new_size, alignment);

    // Attempt to reallocate in place
    void *new_ptr = realloc(ptr, new_size);

    // Check if the new memory block is still aligned
    if (((uintptr_t) new_ptr & (alignment - 1)) == 0)
    {
        return new_ptr; // Memory is properly aligned, return it.
    }

    // Realloc misaligned it, allocate a new aligned block and copy memory
    void *aligned_ptr = aligned_alloc(alignment, new_size);
    if (!aligned_ptr)
        return NULL;

    if (ptr)
    {
        memcpy(aligned_ptr, ptr, new_size);
        free(ptr);
    }
    return aligned_ptr;
#endif
}

static void general_reset(lum_allocator *self)
{
    (void) self;
#ifdef USE_MIMALLOC
    mi_collect(true); // Suggests aggressive cleanup in mimalloc
#endif
}

lum_allocator *lum_create_default_allocator()
{
    lum_allocator *allocator =
        (lum_allocator *) general_alloc(NULL, sizeof(lum_allocator), _Alignof(lum_allocator));
    if (!allocator)
        return NULL;
    allocator->alloc     = general_alloc;
    allocator->free      = general_free;
    allocator->realloc   = general_realloc;
    allocator->reset     = general_reset;
    allocator->user_data = NULL;
    return allocator;
}

/**
 * Frees the allocator instance
 */
void lum_allocator_destroy(lum_allocator *allocator)
{
    if (!allocator)
        return;
    general_free(NULL, allocator);
}