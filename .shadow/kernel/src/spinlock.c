
#include "spinlock.h"
//static inline int atomic_xchg(volatile int *addr, int newval) {
    //int result;
    //asm volatile ("lock xchg %0, %1":
    //"+m"(*addr), "=a"(result) : "1"(newval) : "memory");
    //return result;
//}

void spin_lock(spinlock_t *lk) {
    while (1) {
        intptr_t value = atomic_xchg(lk, 1);
        if (value == 0) {
            break;
        }
    }
}
void spin_unlock(spinlock_t *lk) {
    atomic_xchg(lk, 0);
}
