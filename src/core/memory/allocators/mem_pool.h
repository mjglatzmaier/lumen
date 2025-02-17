#ifndef LUM_MEM_POOL_H
#define LUM_MEM_POOL_H

#include "mem_alloc.h"

#include <stddef.h>
#include <stdint.h>

// Pool allocator structure
typedef struct
{
    size_t   object_size;
    size_t   object_count;
    uint8_t *used_slots;
    void    *buffer;
    void   **free_list;
    size_t   free_index;
    void (*original_free)(lum_allocator *, void *); // Store original free function
} lum_pool_allocator;

lum_allocator *lum_create_pool_allocator(size_t object_size, size_t object_count);
void           lum_pool_allocator_destroy(lum_allocator *allocator);

#endif // LUM_MEM_POOL_H
