#include "mem_stack.h"

#include "math_bits.h" // For alignment macros

#include <stdlib.h>
#include <string.h>

#define STACK_ALIGN 16

// Stack allocator internal allocation function
void *lum_stack_alloc(lum_allocator *self, size_t size, size_t alignment)
{
    lum_stack_allocator *stack = (lum_stack_allocator *) self->user_data;

    // Align offset
    size_t aligned_offset = lum_align_up(stack->offset, alignment);
    size_t new_offset     = aligned_offset + size;

    if (new_offset > stack->size)
    {
        return NULL; // Out of memory
    }

    stack->prev_offset = stack->offset;
    stack->offset      = new_offset;
    return stack->buffer + aligned_offset;
}

// Free the most recent allocation (pop)
void lum_stack_pop(lum_allocator *self, void *ptr)
{
    lum_stack_allocator *stack = (lum_stack_allocator *) self->user_data;

    if (ptr >= (void *) stack->buffer && ptr < (void *) (stack->buffer + stack->size))
    {
        stack->offset = stack->prev_offset;
    }
}

// Reset stack allocator (clears all allocations)
void lum_stack_reset(lum_allocator *self)
{
    lum_stack_allocator *stack = (lum_stack_allocator *) self->user_data;
    stack->offset              = 0;
    stack->prev_offset         = 0;
}

// Create a stack allocator
lum_allocator *lum_create_stack_allocator(size_t size)
{

    lum_allocator *allocator = lum_create_default_allocator();
    if (!allocator)
        return NULL;

    lum_stack_allocator *stack =
        allocator->alloc(allocator, sizeof(lum_stack_allocator), _Alignof(lum_stack_allocator));
    if (!stack)
    {
        allocator->free(allocator, allocator);
        return NULL;
    }

    stack->buffer = allocator->alloc(allocator, size, STACK_ALIGN);
    if (!stack->buffer)
    {
        allocator->free(allocator, stack);
        allocator->free(allocator, allocator);
        return NULL;
    }

    stack->original_free = allocator->free;
    stack->size          = size;
    stack->offset        = 0;
    stack->prev_offset   = 0;
    allocator->alloc     = lum_stack_alloc;
    allocator->free      = lum_stack_pop;
    allocator->reset     = lum_stack_reset;
    allocator->realloc   = NULL; // Not supported
    allocator->user_data = stack;
    return allocator;
}

// Destroy stack allocator
void lum_destroy_stack_allocator(lum_allocator *allocator)
{
    if (!allocator)
        return;
    lum_stack_allocator *stack = (lum_stack_allocator *) allocator->user_data;

    if (!stack)
    {
        allocator->free(allocator, allocator);
        return;
    }
    if (stack->buffer)
    {
        stack->original_free(allocator, stack->buffer);
        stack->buffer = NULL;
    }
    // **Restore the original allocator behavior before freeing pool**
    allocator->free      = stack->original_free;
    allocator->user_data = NULL;
    allocator->free(allocator, stack);
    allocator->free(allocator, allocator);
}
