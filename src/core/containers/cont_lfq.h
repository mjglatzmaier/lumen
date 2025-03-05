#ifndef LUM_CONT_LOCK_FREE_QUEUE_H
#define LUM_CONT_LOCK_FREE_QUEUE_H

#include "allocators/mem_alloc.h"
#include "platform.h"
#include "math/math_bits.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

typedef struct {
    size_t index;  // Queue head index
    size_t version; // Version counter
} tagged_pointer_t;

// Defines a lockless queue type
typedef struct {
    atomic_size_t tail;
    atomic_size_t capacity;
    void **buffer;
    lum_allocator *allocator;
    atomic_uintptr_t head; // Atomic tagged pointer
} lum_lfq_t;

// Initialize queue with given capacity (stack or heap)
static inline bool lum_lfq_init(lum_lfq_t *queue, size_t capacity, lum_allocator *allocator)
{
    if (!queue || !allocator)
        return false;

    if (!lum_is_power_of_two(capacity))
        capacity = 1 << lum_log2_ceil(capacity);

    if (capacity < 2)
        return false; // Minimum capacity is 2

    queue->capacity  = capacity;
    queue->allocator = allocator;
    queue->buffer    = allocator->alloc(allocator, capacity * sizeof(void *), 16);
    if (!queue->buffer)
        return false;

    atomic_store_explicit(&queue->head, 0, memory_order_relaxed);
    atomic_store_explicit(&queue->tail, 0, memory_order_relaxed);
    return true;
}

// Free heap-allocated queue
static inline void lum_lfq_free(lum_lfq_t *queue)
{
    if (!queue) 
        return;

    // Ensure no other threads are accessing the queue before freeing
    atomic_thread_fence(memory_order_acquire);

    // Free the buffer
    if (queue->buffer)
        queue->allocator->free(queue->allocator, queue->buffer);

    // Store allocator before freeing queue
    lum_allocator *allocator = queue->allocator;

    // Free the queue itself
    allocator->free(allocator, queue);

    // Now, free the allocator ONLY if it's private to the queue
    if (allocator)
        allocator->free(allocator, allocator);  // Dangerous if allocator is shared
}


// static inline void lum_lfq_free(lum_lfq_t *queue)
// {
//     if (queue && queue->buffer)
//         queue->allocator->free(queue->allocator, queue->buffer);

//     // Free the queue
//     lum_allocator *allocator = queue->allocator;
//     if (queue)
//     {
//         allocator->free(allocator, queue);
//         queue = NULL;
//     }

//     // Free the allocator
//     if (allocator)
//     {
//         allocator->free(allocator, allocator);
//         allocator = NULL;
//     }
// }

// Check if queue is empty
static inline bool lum_lfq_empty(lum_lfq_t *queue)
{
    assert(queue != NULL);
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    return head == atomic_load_explicit(&queue->tail, memory_order_relaxed);
}

// Check if queue is full
static inline bool lum_lfq_full(lum_lfq_t *queue)
{
    assert(queue != NULL);
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    return ((atomic_load_explicit(&queue->tail, memory_order_relaxed) + 1) & (queue->capacity - 1)) == head;
}


// Push to queue
static inline bool lum_lfq_enqueue(lum_lfq_t *queue, void *item)
{
    tagged_pointer_t old_tail, new_tail;
    uintptr_t packed_new_tail, packed_tail;
    
    do {
        packed_tail = atomic_load(&queue->tail);
        old_tail.index = packed_tail & 0xFFFFFFFF;          // Extract index
        old_tail.version = (packed_tail >> 32) & 0xFFFFFFFF; // Extract version

        size_t head = atomic_load(&queue->head);
        size_t next_tail = (old_tail.index + 1) & (queue->capacity - 1); // capacity is power of 2

        if (next_tail == head)
            return false; // Queue is full

        // Compute new tail with incremented index and version tag
        new_tail.index = next_tail;
        new_tail.version = old_tail.version + 1;

        packed_new_tail = (new_tail.version << 32) | new_tail.index;

        // **Ensure that item is stored AFTER CAS succeeds**
    } while (!atomic_compare_exchange_weak(&queue->tail, &packed_tail, packed_new_tail));

    // Now safe to write the item
    queue->buffer[old_tail.index] = item;

    return true;
}


static inline void *lum_lfq_dequeue(lum_lfq_t *queue)
{
    if (!queue)
        return NULL; // Prevent invalid access

    tagged_pointer_t old_head, new_head;
    uintptr_t packed_new_head, packed_head;
    void *item;

    do {
        packed_head = atomic_load(&queue->head);
        old_head.index = packed_head & 0xFFFFFFFF;           // Extract index
        old_head.version = (packed_head >> 32) & 0xFFFFFFFF; // Extract version

        size_t tail = atomic_load(&queue->tail);
        if (old_head.index == tail) {
            return NULL; // Queue is empty
        }

        item = queue->buffer[old_head.index & (queue->capacity - 1)];

        // Compute new head with incremented index and version tag
        new_head.index = (old_head.index + 1) & (queue->capacity - 1);
        new_head.version = old_head.version + 1;

        packed_new_head = (new_head.version << 32) | new_head.index;

    } while (!atomic_compare_exchange_weak(&queue->head, &packed_head, packed_new_head));

    return item;
}

#endif // LUM_CONT_LOCK_FREE_QUEUE_H
