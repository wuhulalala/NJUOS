#include "spinlock.h"
#include <klib-macros.h>
static inline int atomic_xchg(volatile int *addr, int newval) {
    int result;
    asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "memory");
    return result;
}
void spin_lock(spinlock_t *lk) {
    while (1) {
        intptr_t value = atomic_xchg(lk, LOCKED);
        if (value == UNLOCKED) {
            break;
        }
    }
}
void spin_unlock(spinlock_t *lk) {
    panic_on(atomic_xchg(lk, UNLOCKED) != LOCKED, "Failed to release lock\n");
}

intptr_t try_lock(spinlock_t *lk) {
    return atomic_xchg(lk, LOCKED);
}