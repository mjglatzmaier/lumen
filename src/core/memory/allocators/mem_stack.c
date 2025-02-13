#include "mem_stack.h"
#include <stdlib.h>
#include <string.h>
#include "math_bits.h"  // For alignment macros

// Stack allocator internal allocation function
void* lum_stack_alloc(lum_allocator* self, size_t size, size_t alignment) {
    lum_stack_allocator* stack = (lum_stack_allocator*)self->user_data;

    // Align offset
    size_t aligned_offset = lum_align_up(stack->offset, alignment);
    size_t new_offset = aligned_offset + size;

    if (new_offset > stack->size) {
        return NULL;  // Out of memory
    }
    
    stack->prev_offset = stack->offset;
    stack->offset = new_offset;
    return stack->buffer + aligned_offset;
}

// Free the most recent allocation (pop)
void lum_stack_pop(lum_allocator* self, void* ptr) {
    lum_stack_allocator* stack = (lum_stack_allocator*)self->user_data;

    if (ptr >= (void*)stack->buffer && ptr < (void*)(stack->buffer + stack->size)) {
        stack->offset = stack->prev_offset;
    }
}

// Reset stack allocator (clears all allocations)
void lum_stack_reset(lum_allocator* self) {
    lum_stack_allocator* stack = (lum_stack_allocator*)self->user_data;
    stack->offset = 0;
    stack->prev_offset = 0;
}

// Create a stack allocator
lum_allocator* lum_create_stack_allocator(size_t size) {

    lum_allocator* allocator = (lum_allocator*)lum_get_default_allocator()->alloc(
        lum_get_default_allocator(), sizeof(lum_allocator), _Alignof(lum_allocator)
    );
    
    lum_stack_allocator* stack = (lum_stack_allocator*)lum_get_default_allocator()->alloc(
        lum_get_default_allocator(), sizeof(lum_stack_allocator), _Alignof(lum_stack_allocator)
    );

    stack->buffer = (char*)lum_get_default_allocator()->alloc(
        lum_get_default_allocator(), size, 16 // Align to 16 bytes for SIMD
    );
    stack->size = size;
    stack->offset = 0;
    stack->prev_offset = 0;

    allocator->alloc = lum_stack_alloc;
    allocator->free = lum_stack_pop;
    allocator->reset = lum_stack_reset;
    allocator->realloc = NULL;  // Not supported
    allocator->user_data = stack;

    return allocator;
}

// Destroy stack allocator
void lum_destroy_stack_allocator(lum_allocator* allocator) {
    if (!allocator)
        return;
    lum_stack_allocator* stack = (lum_stack_allocator*)allocator->user_data;

    if (!stack) 
        return;
    
    // Use the allocator's free function if it's available
    if (allocator->free) {
        allocator->free(allocator, stack->buffer);
        allocator->free(allocator, stack);
        allocator->free(allocator, allocator);
    } else {
        // Fallback to system free (shouldn't happen if allocator is valid)
        free(stack->buffer);
        free(stack);
        free(allocator);
    }
}
