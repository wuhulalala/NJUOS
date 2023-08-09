typedef int spinlock_t;
typedef long int intptr_t;
#define SPIN_INIT() 0
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);