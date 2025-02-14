#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "mem_alloc.h"
#include "mem_pool.h"
#include "math_bits.h"

#define POOL_ALIGN 16

// Allocate an object from the pool
static void* lum_pool_allocator_alloc(lum_allocator* allocator, size_t unused_size, size_t unused_align) {    
    (void)unused_size;
    (void)unused_align;

    if (!allocator)
        return NULL;

    lum_pool_allocator* pool = (lum_pool_allocator*) allocator->user_data;
    if (!pool)
        return NULL;

    if (!pool || pool->free_index >= pool->object_count) {
        return NULL; // Pool exhausted
    }

    void* obj = pool->free_list[pool->free_index];

    // Mark as used in the bitmap
    size_t index = pool->free_index;
    pool->used_slots[index / 8] |= (1 << (index % 8));

    pool->free_index++;
    return obj;
}

// Free an object back to the pool
static void lum_pool_allocator_free(lum_allocator* allocator, void* ptr) {
    if (!allocator)
        return;

    lum_pool_allocator* pool = (lum_pool_allocator*) allocator->user_data;
    if (!pool || pool->free_index == 0)
        return;

    // Calculate index in the buffer
    size_t index = ((char*)ptr - (char*)pool->buffer) / pool->object_size;

    // Check if already freed (double free check)
    if (!(pool->used_slots[index / 8] & (1 << (index % 8)))) {
        return; // Already freed
    }

    // Mark as free in bitmap
    pool->used_slots[index / 8] &= ~(1 << (index % 8));
    pool->free_list[--pool->free_index] = ptr;
}

// Initialize the pool allocator
lum_allocator* lum_create_pool_allocator(size_t object_size, size_t object_count) {

    lum_allocator* allocator = lum_create_default_allocator();
    if (!allocator)
        return NULL;

    lum_pool_allocator* pool = allocator->alloc(allocator,
        sizeof(lum_pool_allocator), _Alignof(lum_pool_allocator));
    if (!pool)
    {
        printf("Failed to create pool from default allocator\n");
        allocator->free(allocator, allocator);
        return NULL;
    }

    // Ensure object size is properly aligned
    pool->object_size = lum_align_up(object_size, POOL_ALIGN);

    // Initialize each pointer.
    pool->buffer = NULL;
    pool->free_list = NULL;
    pool->used_slots = NULL;

    pool->buffer = allocator->alloc(allocator, pool->object_size  * object_count, _Alignof(size_t));
    if (!pool->buffer) {
        printf("Failed to pool buffer from default allocator\n");
        allocator->free(allocator, pool);
        allocator->free(allocator, allocator);
        return NULL;
    }

    // Allocate a bitmap to track free/allocated slots
    size_t bitmap_size = (object_count + 7) / 8;  // 1 bit per object
    if (bitmap_size == 0) {
        printf("WARNING: Bitmap size is zero, setting to 1\n");
        bitmap_size = 1;  // Ensure at least 1 byte is allocated
    }
    pool->used_slots = allocator->alloc(allocator, bitmap_size, _Alignof(size_t));
    if (!pool->used_slots) {
        printf("Failed to created used_slots from default allocator\n");
        allocator->free(allocator, pool->buffer);
        allocator->free(allocator, pool);
        allocator->free(allocator, allocator);
        return NULL;
    }
    memset(pool->used_slots, 0, bitmap_size); // Initialize all slots as free

    pool->free_list = (void**)allocator->alloc(allocator, sizeof(void*) * object_count, _Alignof(void*));
    if (!pool->free_list) {
        printf("Failed to created free_list from default allocator\n");
        allocator->free(allocator, pool->used_slots);
        allocator->free(allocator, pool->buffer);
        allocator->free(allocator, pool);
        allocator->free(allocator, allocator);
        return NULL;
    }

    pool->object_count  = object_count;
    pool->free_index    = 0;

     // Save original free method - necessary to actually delete the pool
    pool->original_free = allocator->free; 
    
    // Initialize free list
    for (size_t i = 0; i < object_count; i++) {
        pool->free_list[i] = (char*)pool->buffer + i * pool->object_size;
    }
    allocator->user_data = (void*)pool;

    allocator->alloc     = lum_pool_allocator_alloc;
    allocator->free      = lum_pool_allocator_free;
    allocator->realloc   = NULL;
    return allocator;
}

// Destroy the pool allocator
void lum_pool_allocator_destroy(lum_allocator* allocator) {
    if (!allocator) 
        return;

    lum_pool_allocator* pool = (lum_pool_allocator*) allocator->user_data;
    if (!pool) {
        printf("Pool is NULL, freeing allocator itself.\n");
        allocator->free(allocator, allocator);
        return;
    }
    // Use the original free function for all internal allocations
    if (pool->buffer) {
        pool->original_free(allocator, pool->buffer);
        pool->buffer = NULL;
    }
    if (pool->free_list) {
        pool->original_free(allocator, pool->free_list);
        pool->free_list = NULL;
    }
    if (pool->used_slots) {
        pool->original_free(allocator, pool->used_slots);
        pool->used_slots = NULL;
    }

    // **Restore the original allocator behavior before freeing pool**
    allocator->free = pool->original_free;
    allocator->user_data = NULL;
    allocator->free(allocator, pool);
    allocator->free(allocator, allocator);
}
