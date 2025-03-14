#include "math_hash.h"

#include "../hash/metrohash.h"
#include "../hash/murmurhash.h"
#include "../hash/xxhash.h"


uint64_t lum_hash_metro(const void *key, size_t len) {
    uint8_t out[8];
    metrohash64_1((const uint8_t *)key, len, 0, out);
    return read_u64(out); 
}

uint64_t lum_hash_murmur(const void *key, size_t len) {
    return murmurhash((const char *)key, (uint32_t)len, 0x811c9dc5);
}

uint64_t lum_hash_xxhash(const void *key, size_t len) {
    return XXH64(key, len, 0x811c9dc5);
}