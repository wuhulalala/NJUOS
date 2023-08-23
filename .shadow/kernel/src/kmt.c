#include <os.h>

spinlock_t irq_lk;
static void kmt_init() {
    kmt -> spin_init(&irq_lk, "irq queue lock");
}
static void kmt_spin_init(spinlock_t *lk, const char *name) {
    strcpy(lk -> name, name);
    lk -> lock = KMT_UNLOCK;

}

static void kmt_spin_lock(spinlock_t *lk) {
    bool interrupt_status = ienabled();
    iset(false);

    while (atomic_xchg(&(lk -> lock), KMT_LOCK) == KMT_LOCK);

    lk -> saved_interrupt_status = interrupt_status;
}

static void kmt_spin_unlock(spinlock_t *lk) {
    bool interrupt_status = lk -> saved_interrupt_status;
    panic_on(atomic_xchg(&(lk -> lock), KMT_UNLOCK) == KMT_UNLOCK, "error, does not get the lock");
    iset(interrupt_status);
}

MODULE_DEF(kmt) = {
    .init = kmt_init,
    .spin_init = kmt_spin_init,
    .spin_lock = kmt_spin_lock,
    .spin_unlock = kmt_spin_unlock,
};
