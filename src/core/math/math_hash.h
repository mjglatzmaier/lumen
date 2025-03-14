#ifndef LUM_MATH_HASH_H
#define LUM_MATH_HASH_H

#include "platform.h"

uint64_t lum_hash_murmur(const void *key, size_t len);

// TODO: test endianness of this hash function
uint64_t lum_hash_metro(const void *key, size_t len);

uint64_t lum_hash_xxhash(const void *key, size_t len);

#endif