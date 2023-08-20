#include "spinlock.h"
#ifndef TEST
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
#else
void spin_lock(spinlock_t *lk)   { pthread_mutex_lock(lk); }
void spin_unlock(spinlock_t *lk) { pthread_mutex_unlock(lk); }
int  try_lock(spinlock_t *lk) { return pthread_mutex_trylock(lk) == 0 ? UNLOCKED : LOCKED; }
#endif