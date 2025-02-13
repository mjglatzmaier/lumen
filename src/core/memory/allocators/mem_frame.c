#include "mem_frame.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "math_bits.h"
#include "mem_alloc.h"


// Allocate Memory (Linear Allocator)
static void* frame_alloc(lum_allocator* self, size_t size, size_t alignment) {
    lum_frame_allocator* frame = (lum_frame_allocator*)self->user_data;
    size_t aligned_offset = lum_align_up(frame->offset, alignment);
    
    if (aligned_offset + size > frame->size) {
        return NULL; // Out of memory!
    }

    frame->offset = aligned_offset + size;
    return frame->buffer + aligned_offset;
}

// Reset Allocator (Single Call to Free Memory)
static void frame_reset(lum_allocator* self) {
    lum_frame_allocator* frame = (lum_frame_allocator*)self->user_data;
    frame->offset = 0;  // Just reset the offset
}

//  No individual free (memory freed all at once)
static void frame_free(lum_allocator* self, void* ptr) {
    (void)self; (void)ptr;
}

// No realloc (allocations are fixed)
static void* frame_realloc(lum_allocator* self, void* ptr, size_t new_size) {
    (void)self; (void)ptr; (void)new_size;
    return NULL;
}

// Create a Frame Allocator
lum_allocator* lum_create_frame_allocator(size_t size) {
    lum_allocator* allocator = (lum_allocator*)lum_get_default_allocator()->alloc(
        lum_get_default_allocator(), sizeof(lum_allocator), _Alignof(lum_allocator)
    );
    
    lum_frame_allocator* frame = (lum_frame_allocator*)lum_get_default_allocator()->alloc(
        lum_get_default_allocator(), sizeof(lum_frame_allocator), _Alignof(lum_frame_allocator)
    );

    frame->buffer = (char*)lum_get_default_allocator()->alloc(
        lum_get_default_allocator(), size, 16 // Align to 16 bytes for SIMD
    );

    frame->size             = size;
    frame->offset           = 0;
    allocator->alloc        = frame_alloc;
    allocator->free         = frame_free;
    allocator->realloc      = frame_realloc;
    allocator->reset        = frame_reset;
    allocator->user_data    = frame;

    return allocator;
}

// Destroy Frame Allocator (Free Entire Buffer)
void lum_destroy_frame_allocator(lum_allocator* allocator) {
    if (!allocator)
        return;

    lum_frame_allocator* frame = (lum_frame_allocator*)allocator->user_data;

    if (!frame) 
        return;

    // Use the allocator's free function if it's available
    if (allocator->free) {
        allocator->free(allocator, frame->buffer);
        allocator->free(allocator, frame);
        allocator->free(allocator, allocator);
    } else {
        // Fallback to system free (shouldn't happen if allocator is valid)
        free(frame->buffer);
        free(frame);
        free(allocator);
    }
}