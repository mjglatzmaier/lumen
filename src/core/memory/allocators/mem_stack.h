#ifndef LUM_STACK_ALLOC_H
#define LUM_STACK_ALLOC_H

#include <stddef.h>
#include "mem_alloc.h"  // Reuse our existing allocator API

typedef struct {
    char* buffer;       // Stack memory buffer
    size_t size;        // Total size of buffer
    size_t offset;      // Current allocation offset
    size_t prev_offset; // Previous allocation offset (for checkpointing)
    void (*original_free)(lum_allocator*, void*); 
} lum_stack_allocator;

// Create a new stack allocator
lum_allocator* lum_create_stack_allocator(size_t size);

// Allocate memory from stack (aligned)
void* lum_stack_alloc(lum_allocator* self, size_t size, size_t alignment);

// Free the most recent allocation (pop)
void lum_stack_pop(lum_allocator* self, void* ptr);

// Reset the stack (clear all allocations)
void lum_stack_reset(lum_allocator* self);

// Destroy the stack allocator
void lum_destroy_stack_allocator(lum_allocator* self);

#endif
