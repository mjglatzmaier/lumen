#include "cont_hm.h"
#include "../math/math_hash.h"

lum_hm *lum_hm_create(size_t capacity, lum_hash_func hash_func, lum_allocator *allocator) {
    if (!allocator) {
        allocator = lum_create_default_allocator();
    }
    lum_hm *hm = allocator->realloc(allocator, NULL, sizeof(lum_hm), DHM_ALIGN);
    if (!hm) 
        return NULL;

    hm->capacity = (capacity > 0) ? capacity : 16;
    hm->count = 0;
    hm->allocator = allocator;
    hm->hash_func = hash_func ? hash_func : lum_hash_murmur;
    hm->entries = allocator->realloc(allocator, NULL, sizeof(lum_hm_entry) * hm->capacity, DHM_ALIGN);
    if (!hm->entries) {
        allocator->free(allocator, hm);
        return NULL;
    }
    for (size_t i = 0; i < hm->capacity; i++) {
        hm->entries[i].status = 0;
    }
    return hm;
}

void lum_hm_resize(lum_hm *hm, size_t new_capacity) {
    if (!hm || new_capacity <= hm->capacity)
        return;

    lum_hm_entry *new_entries = hm->allocator->alloc(hm->allocator, sizeof(lum_hm_entry) * new_capacity, DHM_ALIGN);
    if (!new_entries) 
        return;
    for (size_t i = 0; i < new_capacity; i++) {
        new_entries[i].status = 0;  // 0 = Empty
    }

    // Rehash all existing entries into new table
    for (size_t i = 0; i < hm->capacity; i++) {
        if (hm->entries[i].status == 1) {  // Only move active entries
            size_t index = hm->entries[i].hash % new_capacity;
            while (new_entries[index].status == 1) {
                index = (index + 1) % new_capacity;
            }
            new_entries[index] = hm->entries[i];  // Move entry
        }
    }
    hm->allocator->free(hm->allocator, hm->entries);
    hm->entries = new_entries;
    hm->capacity = new_capacity;
}


void lum_hm_free(lum_hm *hm) {
    if (!hm)
        return;
    lum_allocator *allocator = hm->allocator;
    for (size_t i = 0; i < hm->capacity; i++) {
        if (hm->entries[i].status == 1) {  // Only free occupied entries
            allocator->free(allocator, hm->entries[i].key);
            allocator->free(allocator, hm->entries[i].value);
        }
    }
    allocator->free(allocator, hm->entries);
    allocator->free(allocator, hm);
    lum_allocator_destroy(allocator);
}
