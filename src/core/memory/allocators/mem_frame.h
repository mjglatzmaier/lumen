#ifndef LUM_MEM_FRAME_H
#define LUM_MEM_FRAME_H

#include "mem_alloc.h"
#include <stddef.h>

// Frame Allocator Structure
typedef struct {
    char* buffer;    // Pre-allocated memory block
    size_t size;     // Total memory size
    size_t offset;   // Current allocation offset
    void (*original_free)(lum_allocator*, void*); 
} lum_frame_allocator;

lum_allocator* lum_create_frame_allocator(size_t size);
void lum_destroy_frame_allocator(lum_allocator* allocator);

#endif // LUM_MEM_FRAME_H
