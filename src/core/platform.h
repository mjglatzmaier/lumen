#ifndef LUMEN_PLATFORM_H
#define LUMEN_PLATFORM_H

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM_MACOS
#elif defined(__linux__)
#define PLATFORM_LINUX
#else
#error "Unknown platform!"
#endif

// Vulkan extensions based on platform
#ifdef PLATFORM_MACOS
#define ENABLE_PORTABILITY_EXTENSIONS 1
#else
#define ENABLE_PORTABILITY_EXTENSIONS 0
#endif

// --------------- Threading ------------------- //
#ifdef PLATFORM_WINDOWS
#include <windows.h>
typedef HANDLE            lum_thread;
typedef DWORD             lum_thread_id;
typedef CRITICAL_SECTION  lum_mutex;
typedef ConditionVariable lum_cond_var;
#else
#include <pthread.h>
#include <sched.h>
#include <unistd.h> // POSIX usleep
#include <stdint.h>
typedef pthread_t       lum_thread;
typedef int             lum_thread_id;
typedef pthread_mutex_t lum_mutex;
typedef pthread_cond_t  lum_cond_var;
#endif

/**
 * @brief Sleep the current thread for a specified number of milliseconds.
 * @param milliseconds The duration to sleep.
 */
static inline void lum_thread_sleep(uint32_t milliseconds)
{
    if (milliseconds == 0)
    {
        return; // No-op for zero sleep
    }

#ifdef PLATFORM_WINDOWS
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000); // Faster than nanosleep
#endif
}

static inline void lum_thread_yield()
{
#ifdef PLATFORM_WINDOWS
    SwitchToThread(); // Windows equivalent
#else
    sched_yield(); // POSIX (Linux/macOS)
#endif
}

// Thread function type
typedef void *(*lum_thread_func)(void *);

// Thread creation
static inline lum_thread lum_thread_create(lum_thread_func func, void *arg)
{
#ifdef PLATFORM_WINDOWS
    return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, NULL);
#else
    lum_thread thread;
    pthread_create(&thread, NULL, func, arg);
    return thread;
#endif
}

static inline void lum_thread_join(lum_thread thread)
{
#ifdef PLATFORM_WINDOWS
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
#else
    pthread_join(thread, NULL);
#endif
}

// Mutex functions
static inline void lum_mutex_init(lum_mutex *mutex)
{
#ifdef PLATFORM_WINDOWS
    InitializeCriticalSection(mutex);
#else
    pthread_mutex_init(mutex, NULL);
#endif
}

static inline void lum_cond_init(lum_cond_var *var)
{
#ifdef PLATFORM_WINDOWS
    InitializeConditionVariable(var);
#else
    pthread_cond_init(var, NULL);
#endif
}

static inline void lum_cond_destroy(lum_cond_var *var)
{
#ifdef PLATFORM_WINDOWS
    // No-op: Windows condition variables do not need explicit destruction
#else
    pthread_cond_destroy(var);
#endif
}

static inline void lum_cond_signal(lum_cond_var *var)
{
#ifdef PLATFORM_WINDOWS
    WakeConditionVariable(var);
#else
    pthread_cond_signal(var);
#endif
}

static inline void lum_cond_broadcast(lum_cond_var *var)
{
#ifdef PLATFORM_WINDOWS
    WakeAllConditionVariable(var);
#else
    pthread_cond_broadcast(var);
#endif
}

static inline void lum_cond_wait(lum_cond_var *var, lum_mutex *mutex)
{
#ifdef PLATFORM_WINDOWS
    SleepConditionVariableCS(var, mutex, INFINITE);
#else
    pthread_cond_wait(var, mutex);
#endif
}

static inline void lum_mutex_lock(lum_mutex *mutex)
{
#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(mutex);
#endif
}

static inline void lum_mutex_unlock(lum_mutex *mutex)
{
#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(mutex);
#endif
}

static inline void lum_mutex_destroy(lum_mutex *mutex)
{
#ifdef PLATFORM_WINDOWS
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(mutex);
#endif
}

#if defined(PLATFORM_WINDOWS)
#define THREAD_LOCAL __declspec(thread)
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
#define THREAD_LOCAL __thread
#else
#error "Compiler does not support thread-local storage"
#endif

// --------------- End Threading ------------------- //

// ---------------- Memory --------------------- //
#ifdef PLATFORM_WINDOWS
#define CACHE_ALIGNED __declspec(align(64))
#else
#define CACHE_ALIGNED __attribute__((aligned(64)))
#endif
// --------------- End Memory ------------------ //

#endif // LUMEN_PLATFORM_H
