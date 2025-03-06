#ifndef LUM_CONT_DA_H
#define LUM_CONT_DA_H

#define STB_DS_IMPLEMENTATION
#include "allocators/mem_alloc.h"
#include "allocators/mem_pool.h"
#include "platform.h"

#include <stddef.h>

#define DARR_ALIGN 16

// Thread-local default allocator
static THREAD_LOCAL lum_allocator *thread_local_allocator = NULL;

// Set the thread-local allocator safely
static inline void lum_set_thread_local_allocator(lum_allocator *allocator)
{
    thread_local_allocator = allocator ? allocator : lum_create_default_allocator();
}

#define STBDS_REALLOC(context, ptr, size)                                                          \
    ((thread_local_allocator)                                                                      \
         ? thread_local_allocator->realloc(thread_local_allocator, ptr, size, DARR_ALIGN)          \
         : NULL)
#define STBDS_FREE(context, ptr)                                                                   \
    ((thread_local_allocator) ? thread_local_allocator->free(thread_local_allocator, ptr) : NULL)

#include "stb_ds.h"

static inline void *lum_create_dynamic_array_internal(size_t elem_size, size_t count,
                                                      lum_allocator *allocator)
{
    if (!allocator)
    {
        allocator = lum_create_pool_allocator(elem_size, count);
    }
    lum_set_thread_local_allocator(allocator);
    return NULL; // `stb_ds` will handle actual memory allocation later
}

// Creates thread local allocator to be used in stb_ds.h
// Example useage: create_dynamic_arr_with_allocator(double, 1024, NULL);
// Will create a pool array with max size 1024 (not dynamic)
// To create a dynamic array pass in the default allocator
#define lum_da_create(type, count, allocator)                                                      \
    ((type *) lum_create_dynamic_array_internal(sizeof(type), count, allocator))

#define lum_da_size(a) stbds_arrlen(a)
#define lum_da_reserve(a, n) stbds_arrgrow(a, n, 1)
#define lum_da_free(a) stbds_arrfree(a)
#define lum_da_pop(a) stbds_arrpop(a)
#define lum_da_app(a, v) stbds_arrput(a, v)

#endif
