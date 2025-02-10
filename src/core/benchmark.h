#ifndef LUMEN_BENCHMARK_H
#define LUMEN_BENCHMARK_H

#include <time.h>

// Returns time in seconds with nanosecond precision
static inline double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

#endif // LUMEN_BENCHMARK_H
