#ifndef TEST
#include <klib.h>
#endif
#include <am.h>
typedef int spinlock_t;
#define SPIN_INIT() 0
#define LOCKED 1
#define UNLOCKED 0
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
uintptr_t try_lock(spinlock_t *lk); 