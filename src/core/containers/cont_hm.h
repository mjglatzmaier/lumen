#ifndef LUM_CONT_HM_H
#define LUM_CONT_HM_H

#include "allocators/mem_alloc.h"
#include "allocators/mem_pool.h"
#include "../math/math_hash.h"
#include "platform.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define DHM_ALIGN 16

typedef uint64_t (*lum_hash_func)(const void *key, size_t len);

typedef struct {
    void *key;           // Pointer to key
    void *value;         // Pointer to value
    uint64_t hash;       // Hash value for the key
    uint8_t status;      // 0 = empty, 1 = occupied, 2 = deleted
} lum_hm_entry;

typedef struct {
    lum_hm_entry *entries;      // Hash table array
    size_t capacity;            // Total allocated size
    size_t count;               // Number of elements stored
    lum_allocator *allocator;   // Custom allocator
    lum_hash_func hash_func;    // The hash function used for this hash map
} lum_hm;

lum_hm *lum_hm_create(size_t capacity, lum_hash_func hash_func, lum_allocator *allocator);
void lum_hm_resize(lum_hm *hm, size_t new_capacity);
void lum_hm_free(lum_hm *hm);

static inline void lum_hm_put_impl(lum_hm *hm, const void *key, size_t key_size, const void *value, size_t value_size) {
    if (!hm || !key)
        return;
    uint64_t hash = hm->hash_func(key, key_size);
    size_t index = hash % hm->capacity;
    size_t i = 0;
    while (hm->entries[index].status == 1) {
        if (memcmp(hm->entries[index].key, key, key_size) == 0) {
            memcpy(hm->entries[index].value, value, value_size);
            return;
        }
        index = (index + (i * i)) % hm->capacity;  // Quadratic probing
        i++;
        //index = (index + 1) % hm->capacity;
    }
    // Insert new entry
    hm->entries[index].key = hm->allocator->alloc(hm->allocator, key_size, DHM_ALIGN);
    memcpy(hm->entries[index].key, key, key_size);
    hm->entries[index].value = hm->allocator->alloc(hm->allocator, value_size, DHM_ALIGN);
    memcpy(hm->entries[index].value, value, value_size);
    hm->entries[index].status = 1;
    hm->entries[index].hash = hash;
    hm->count++;
    if (hm->count > (hm->capacity * 0.75)) {
        printf(" Resizing hashmap...\n");
        lum_hm_resize(hm, hm->capacity * 2);
    }
}

static inline bool lum_hm_get_impl(lum_hm *hm, const void *key, size_t key_size, void *out_value, size_t value_size) {
    if (!hm || !key || !out_value)
        return false;
    uint64_t hash = hm->hash_func(key, key_size);
    size_t index = hash % hm->capacity;
    size_t i = 0;
    while (hm->entries[index].status != 0) {
        if (hm->entries[index].status == 1 && 
            memcmp(hm->entries[index].key, key, key_size) == 0) {
            memcpy(out_value, hm->entries[index].value, value_size);
            return true;
        }
        index = (index + (i * i)) % hm->capacity;  // Quadratic probing
        i++;
        //index = (index + 1) % hm->capacity; // Linear probing
    }
    return false;
}

static inline void lum_hm_remove_impl(lum_hm *hm, const void *key, size_t key_size) {
    if (!hm || !key)
        return;
    uint64_t hash = hm->hash_func(key, key_size);
    size_t index = hash % hm->capacity;
    while (hm->entries[index].status != 0) {  
        if (hm->entries[index].status == 1 &&
            memcmp(hm->entries[index].key, key, key_size) == 0) {
            hm->allocator->free(hm->allocator, hm->entries[index].key);
            hm->allocator->free(hm->allocator, hm->entries[index].value);
            hm->entries[index].status = 2;  
            hm->entries[index].key = NULL;
            hm->entries[index].value = NULL;
            hm->count--;
            return;
        }
        index = (index + 1) % hm->capacity;
    }
}

#define lum_hm_put(hm, key, value) \
    lum_hm_put_impl((hm), &(key), sizeof(key), &(value), sizeof(value))

#define lum_hm_get(hm, key, out_value) \
    lum_hm_get_impl((hm), &(key), sizeof(key), &(out_value), sizeof(out_value))

#define lum_hm_remove(hm, key) \
    lum_hm_remove_impl((hm), &(key), sizeof(key))

#endif