#ifndef LUM_CONT_DA_H
#define LUM_CONT_DA_H

#include "allocators/mem_alloc.h"
#include "allocators/mem_pool.h"
#include "platform.h"

#include <stddef.h>
#include <stdio.h>

#define DARR_ALIGN 16

typedef struct {
    void *data;          // Start of allocated memory
    void *end;           // End of allocated memory (capacity limit)
    void *next;          // Pointer to next free slot
    size_t elem_size;    // Size of each element
    size_t length;
    lum_allocator *allocator;  // Allocator used for this array
} lum_da;

#define lum_da_data(da, type) ((type *)(da->data))
lum_da *lum_da_create(size_t elem_size, size_t capacity, lum_allocator *allocator);
size_t lum_da_size(lum_da *da);
void lum_da_free(lum_da *da);

static inline void lum_da_grow(lum_da *da) {
    size_t old_capacity = (size_t)((char *)da->end - (char *)da->data) / da->elem_size;
    size_t new_capacity = old_capacity * 2 + 1;
    ptrdiff_t index_offset = (char *)da->next - (char *)da->data;
    void *new_data = da->allocator->realloc(da->allocator, da->data, new_capacity * da->elem_size, DARR_ALIGN);
    if (!new_data)
        return;
    da->data = new_data;
    da->next = (char *)new_data + index_offset;
    da->end = (char *)new_data + (new_capacity * da->elem_size);
}

#define lum_da_app(da, value)                                 \
    do {                                                      \
        if ((da)->next >= (da)->end) {                        \
            lum_da_grow(da);                                  \
        }                                                     \
        ((typeof(value) *)(da)->next)[0] = (value);           \
        (da)->next = (char *)(da)->next + (da)->elem_size;    \
    } while (0)


#endif // LUM_CONT_DA_H
