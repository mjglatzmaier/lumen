#include "cont_da.h"


lum_da *lum_da_create(size_t elem_size, size_t capacity, lum_allocator *allocator) {
    if (!allocator) {
        allocator = lum_create_pool_allocator(elem_size, capacity);
    }

    lum_da *da = allocator->realloc(allocator, NULL, sizeof(lum_da), DARR_ALIGN);
    if (!da) return NULL;

    da->data = allocator->realloc(allocator, NULL, capacity * elem_size, DARR_ALIGN);
    if (!da->data) {
        allocator->free(allocator, da);
        return NULL;
    }

    da->next = da->data;
    da->length = capacity;
    da->end = (char *)da->data + (capacity * elem_size);
    da->elem_size = elem_size;
    da->allocator = allocator;
    return da;
}

void lum_da_free(lum_da *da) {
    if (!da) return;
    da->allocator->free(da->allocator, da->data);
    da->allocator->free(da->allocator, da);
}

size_t lum_da_size(lum_da *da) {
    return (da) ? (da)->length : 0;
}
