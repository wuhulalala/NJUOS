#include "slabs.h"

void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
uintptr_t try_lock(spinlock_t *lk); 
void slabs_init() {
    slabs = (Chunk*)((uintptr_t)buddys + (uintptr_t)(sizeof(Chunk) * buddys_size));
    assert(slabs);
    slabs_size = log_n(PGSIZE);
    assert(slabs_size == 12);
    Chunk *slabs_end = (Chunk*)((uintptr_t)slabs + (uintptr_t)(sizeof(Chunk) * slabs_size * cpu_count()));
    assert((uintptr_t)slabs_end <= (uintptr_t)chunks_base);
    for (int i = 0; i < cpu_count(); i++) {
        Chunk *slabs_i = (Chunk *) ((uintptr_t)slabs + i * (uintptr_t)(sizeof(Chunk) * slabs_size));
        for (int j = 0; j < slabs_size; j++) {
            slabs_i[j].next = slabs_i[j].prev = &slabs_i[j];
            assert(((uintptr_t)(&slabs_i[j]) - (uintptr_t)slabs_i) <= (uintptr_t)(sizeof(Chunk) * slabs_size));
            #ifdef TEST
            pthread_mutex_init(&slabs_i[j].lk, NULL);
            #else
            slabs_i[j].lk = SPIN_INIT();
            #endif
        }
    }

    printf("slabs init finished\n");

}

uintptr_t *slabs_malloc(size_t n) {
    return NULL;
}

void slabs_free(uintptr_t * pointer) {

}