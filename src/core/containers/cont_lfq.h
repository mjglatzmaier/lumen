#ifndef LUM_CONT_LOCK_FREE_QUEUE_H
#define LUM_CONT_LOCK_FREE_QUEUE_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdbool.h>
#include "allocators/mem_alloc.h"


// Defines a lockless queue type
typedef struct {
    _Atomic size_t head;
    _Atomic size_t tail;
    size_t capacity;
    void** buffer;
    lum_allocator* allocator;
} lum_lfq_t;

// Initialize queue with given capacity (stack or heap)
static inline bool lum_lfq_init(lum_lfq_t* queue, size_t capacity, lum_allocator* allocator) {
    queue->capacity = capacity;
    queue->allocator = allocator;
    queue->buffer = allocator->alloc(allocator, capacity * sizeof(void*), 16);
    if (!queue->buffer) 
        return false;

    atomic_store(&queue->head, 0);
    atomic_store(&queue->tail, 0);
    return true;
}

// Free heap-allocated queue
static inline void lum_lfq_free(lum_lfq_t* queue) {
    if (queue && queue->buffer)
        queue->allocator->free(queue->allocator, queue->buffer);

    // Free the queue 
    lum_allocator* allocator = queue->allocator;
    if (queue)
    {
        allocator->free(allocator, queue);
        queue = NULL;
    }

    // Free the allocator
    if (allocator)
    {
        allocator->free(allocator, allocator);
        allocator = NULL;
    }
}

// Check if queue is empty
static inline bool lum_lfq_empty(lum_lfq_t* queue) {
    return atomic_load(&queue->head) == atomic_load(&queue->tail);
}

// Check if queue is full
static inline bool lum_lfq_full(lum_lfq_t* queue) {
    return ((atomic_load(&queue->tail) + 1) % queue->capacity) == atomic_load(&queue->head);
}

// Push to queue
static inline bool lum_lfq_enqueue(lum_lfq_t* queue, void* item) {
    size_t tail, next_tail;
    do {
        tail = atomic_load(&queue->tail);
        next_tail = (tail + 1) % queue->capacity;

        if (next_tail == atomic_load(&queue->head))
            return false; // Full

        // **Write to buffer before modifying tail**
        queue->buffer[tail] = item;

    } while (!atomic_compare_exchange_weak(&queue->tail, &tail, next_tail));

    return true;
}


static inline void *lum_lfq_dequeue(lum_lfq_t *queue) {
    if (!queue) 
        return NULL;  // Prevent invalid access

    size_t head, tail;
    void *item;

    do {
        head = atomic_load(&queue->head);
        tail = atomic_load(&queue->tail);

        if (head == tail) {
            return NULL;  // Queue is empty
        }

        // **Read before modifying head**
        item = queue->buffer[head % queue->capacity];

    } while (!atomic_compare_exchange_weak(&queue->head, &head, (head + 1) % queue->capacity));

    return item;
}


#endif // LUM_CONT_LOCK_FREE_QUEUE_H
