#ifndef LUM_CONT_LOCK_FREE_QUEUE_H
#define LUM_CONT_LOCK_FREE_QUEUE_H

#include "allocators/mem_alloc.h"
#include "platform.h"
#include "math/math_bits.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

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

    // if (!lum_is_power_of_two(capacity))
    //     capacity = 1 << lum_log2_ceil(capacity);

    if (capacity < 2)
        return false; // Minimum capacity is 2

    queue->capacity  = capacity + 1;
    queue->allocator = allocator;
    queue->buffer    = allocator->alloc(allocator, queue->capacity * sizeof(void *), 16);
    if (!queue->buffer)
        return false;

    atomic_store_explicit(&queue->head, 0, memory_order_relaxed);
    atomic_store_explicit(&queue->tail, 0, memory_order_relaxed);
    return true;
}

static inline void lum_lfq_free(lum_lfq_t **queue)
{
    if (!queue || !*queue)
        return;

    lum_lfq_t *q = *queue; // Dereference once to avoid extra derefs

    // Ensure no other threads are accessing the queue before freeing
    atomic_thread_fence(memory_order_acquire);

    // Free the buffer
    if (q && q->buffer)
        q->allocator->free(q->allocator, q->buffer);

    // Store allocator before freeing queue
    lum_allocator *allocator = q->allocator;

    // Free the queue itself
    allocator->free(allocator, q);
    *queue = NULL;

    // Free allocator if it is private to the queue
    if (allocator)
        allocator->free(allocator, allocator);
}

// Check if queue is empty
static inline bool lum_lfq_empty(lum_lfq_t *queue)
{
    assert(queue != NULL);
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&queue->tail, memory_order_relaxed);
    return (head % queue->capacity) == (tail % queue->capacity);
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
    uintptr_t packed_tail, packed_head;
    size_t next_tail;

    do {
        packed_tail = atomic_load(&queue->tail);
        packed_head = atomic_load_explicit(&queue->head, memory_order_acquire);

        size_t tail_index = packed_tail % (queue->capacity - 1);
        size_t head_index = packed_head % (queue->capacity - 1);

        // Full Condition
        next_tail = (packed_tail + 1) % (queue->capacity);
        if (head_index == next_tail) {
            return false; // Queue is full
        }

        // Store the item before updating tail
        queue->buffer[tail_index] = item;

    } while (!atomic_compare_exchange_weak(&queue->tail, &packed_tail, (packed_tail + 1)));

    return true;
}

static inline void *lum_lfq_dequeue(lum_lfq_t *queue)
{
    uintptr_t packed_head, packed_tail;
    size_t next_head;
    void *item;

    do {
        packed_head = atomic_load(&queue->head);
        packed_tail = atomic_load_explicit(&queue->tail, memory_order_acquire);

        size_t head_index = packed_head % (queue->capacity /*- 1*/);
        size_t tail_index = packed_tail % (queue->capacity /*- 1*/);

        // Empty Condition
        if (head_index == tail_index) {
            return NULL; // Queue is empty
        }

        // Read item before updating head
        item = queue->buffer[head_index];

        next_head = (head_index + 1) % queue->capacity;

    } while (!atomic_compare_exchange_weak(&queue->head, &packed_head, packed_head + 1));

    return item;
}


#endif // LUM_CONT_LOCK_FREE_QUEUE_H
