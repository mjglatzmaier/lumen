#include "mem_frame.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "math_bits.h"
#include "mem_alloc.h"

#define FRAME_ALIGN 16

// Allocate Memory (Linear Allocator)
static void* frame_alloc(lum_allocator* self, size_t size, size_t alignment) {
    lum_frame_allocator* frame = (lum_frame_allocator*)self->user_data;
    
    // Ensure base buffer is aligned
    if (((uintptr_t)frame->buffer & (alignment - 1)) != 0) {
        return NULL;  // Misaligned buffer, should never happen.
    }

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
static void* frame_realloc(lum_allocator* self, void* ptr, size_t new_size, size_t alignment) {
    (void)self; (void)ptr; (void)new_size; (void)alignment;
    return NULL;
}

// Create a Frame Allocator
lum_allocator* lum_create_frame_allocator(size_t size) {
    lum_allocator* allocator = lum_create_default_allocator();

    lum_frame_allocator* frame = allocator->alloc(allocator,
         sizeof(lum_frame_allocator), _Alignof(lum_frame_allocator));

    // Align the frame to 16 bytes
    frame->buffer = (char*)allocator->alloc(allocator, size, FRAME_ALIGN);
    if (!frame->buffer) {
        allocator->free(allocator, frame);
        allocator->free(allocator, allocator);
        return NULL;
    }

    frame->size             = size;
    frame->offset           = 0;
    allocator->alloc        = frame_alloc;
    allocator->free         = frame_free;
    allocator->realloc      = frame_realloc;
    allocator->reset        = frame_reset;
    allocator->user_data    = frame;

    assert(((uintptr_t)frame->buffer & (FRAME_ALIGN - 1)) == 0); //&& "Frame buffer is misaligned!");
    return allocator;
}

// Destroy Frame Allocator (Free Entire Buffer)
void lum_destroy_frame_allocator(lum_allocator* allocator) {
    if (!allocator)
        return;

    lum_frame_allocator* frame = (lum_frame_allocator*)allocator->user_data;
    if (!frame) 
    {
        allocator->free(allocator, allocator);
        return;
    }

    // Use the allocator's free function if it's available
    if (frame->buffer)
        allocator->free(allocator, frame->buffer);
    allocator->free(allocator, frame);
    allocator->free(allocator, allocator);
}