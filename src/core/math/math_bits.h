#ifndef LUM_MATH_BITS_H
#define LUM_MATH_BITS_H

#include <stddef.h>
#include <stdint.h>

// ** Alignment Utils**
static inline size_t lum_align_up(size_t value, size_t alignment)
{
    return (value + (alignment - 1)) & ~(alignment - 1);
}

static inline int lum_is_power_of_two(size_t x)
{
    return (x & (x - 1)) == 0;
}

// ** Integer Log Base 2 (for efficient power-of-2 checks)**
static inline int lum_log2_ceil(uint32_t x)
{
    return 32 - __builtin_clz(x - 1);
}

static inline int lum_log2_floor(uint32_t x)
{
    return 31 - __builtin_clz(x);
}

// ** Fast Min/Max using bitwise ops**
static inline int lum_min(int a, int b)
{
    return b ^ ((a ^ b) & -(a < b));
}

static inline int lum_max(int a, int b)
{
    return a ^ ((a ^ b) & -(a < b));
}

// ** Get MSB using LZCNT instruction**
static inline int lum_msb(uint32_t x)
{
#if defined(__LZCNT__)
    return 31 - __builtin_clz(x);
#else
    return x ? 31 - __builtin_clz(x) : -1; // Fallback
#endif
}

// ** Get LSB using TZCNT instruction**
static inline int lum_lsb(uint32_t x)
{
#if defined(__BMI__)
    return __builtin_ctz(x);
#else
    return x ? __builtin_ctz(x) : -1;
#endif
}

// ** Hardware-accelerated POPCOUNT**
static inline int lum_popcount(uint32_t x)
{
#if defined(__POPCNT__) || defined(__AVX2__)
    return __builtin_popcount(x);
#else
    // **Fallback (Naive, O(n) complexity)**
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    return ((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101 >> 24;
#endif
}

// **🚀 Check if a number is even (branchless)**
static inline int lum_is_even(int x)
{
    return !(x & 1);
}

// ** Check if a number is odd (branchless)**
static inline int lum_is_odd(int x)
{
    return x & 1;
}

// ** Reverse Bits in a 32-bit Integer**
static inline uint32_t lum_reverse_bits(uint32_t x)
{
    x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
    x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
    x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
    x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
    x = (x >> 16) | (x << 16);
    return x;
}

// ** Get the Next Power of 2 (Efficient Memory Sizing)**
static inline uint32_t lum_next_power_of_two(uint32_t x)
{
    if (x == 0)
        return 1;
    return 1U << lum_log2_ceil(x);
}

#endif // LUM_MATH_BITS_H
