#include "spinlock.h"
#ifdef TEST
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
typedef pthread_mutex_t spinlock_t;
#define SPIN_INIT() PTHREAD_MUTEX_INITIALIZER
void spin_lock(mutex_t *lk)   { pthread_mutex_lock(lk); }
void spin_unlock(mutex_t *lk) { pthread_mutex_unlock(lk); }

#else
#include <klib-macros.h>
void spin_lock(spinlock_t *lk) {
    while (1) {
        uintptr_t value = atomic_xchg(lk, LOCKED);
        if (value == UNLOCKED) {
            break;
        }
    }
}
void spin_unlock(spinlock_t *lk) {
    assert(atomic_xchg(lk, UNLOCKED) == LOCKED);
}

uintptr_t try_lock(spinlock_t *lk) {
    return atomic_xchg(lk, LOCKED);
}
#endif
