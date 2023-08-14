#include "slabs.h"

// spin_lock function
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
uintptr_t try_lock(spinlock_t *lk); 

// list operate function
void list_insert(Chunk* chunk);
void list_remove(Chunk *chunk); 

// buddy function
uintptr_t *buddys_malloc(size_t n);
void buddys_free(uintptr_t *pointer);

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
    printf("the chunk size is %d, the slabs range is from %p to %p\n", sizeof(Chunk), slabs, slabs_end);

}

uintptr_t *slabs_malloc(size_t n) {
    size_t idx = log_n(n);
    size_t actual_size = mem_request2_size(n);
    assert(actual_size >= MINSIZE && actual_size < PGSIZE);
    int current_cpu = 0;
    int cpu = cpu_current();
    int default_cpu = cpu;
    Chunk *rc = NULL, *slabs_i = NULL;
    do {
        slabs_i = (Chunk*)((uintptr_t)slabs + (uintptr_t)(sizeof(Chunk) * slabs_size * cpu));
        assert(((uintptr_t)slabs_i - (uintptr_t)slabs) == sizeof(Chunk) * slabs_size * cpu);
        if (cpu == cpu_current()) {spin_lock(&slabs_i[idx].lk);}
        else {
            if (try_lock(&slabs_i[idx].lk) == LOCKED) {
                goto Go_to_next_cpu;
            }
        }
        if (&slabs_i[idx] != slabs_i[idx].next) {
            rc = slabs_i[idx].next;
            assert(rc);
            assert(CHUNKS_GET_FLAG_ADD(rc) == CHUNKS_PAGE_SLAB);
            assert(CHUNKS_GET_STATUS_ADD(rc) == CHUNKS_PAGE_INUSE);

            CHUNKS_SET_IDX_ADD(rc, idx);


            assert(CHUNKS_GET_IDX_ADD(rc) == idx);

            list_remove(rc);
            rc -> cpu = cpu;
            spin_unlock(&slabs_i[idx].lk);
            break;
        } else {
            spin_unlock(&slabs_i[idx].lk);
        }

Go_to_next_cpu:
    cpu = (cpu + 1) % cpu_count();

    } while (cpu != default_cpu);

    if (rc == NULL) {
        rc = (uintptr_t *) buddys_malloc(PGSIZE);
        assert(CHUNKS_GET_FLAG_ADD(rc) == CHUNKS_PAGE_BUDDY);
        assert(CHUNKS_GET_STATUS_ADD(rc) == CHUNKS_PAGE_INUSE);

        CHUNKS_SET_IDX_ADD(rc, idx);
        CHUNKS_SET_FLAG_ADD(rc, CHUNKS_PAGE_SLAB);


        assert(CHUNKS_GET_IDX_ADD(rc) == idx);
        assert(CHUNKS_GET_FLAG_ADD(rc) == CHUNKS_PAGE_SLAB);

        for (Chunk *t = (uintptr_t)((uintptr_t)rc + (uintptr_t)(1 << idx)); (uintptr_t)t < (uintptr_t)rc + (uintptr_t)PGSIZE; t = (Chunk *)((uintptr_t)t + ((uintptr_t)1 << idx))) {
            assert((uintptr_t)t + (uintptr_t)(1 << idx) <= (uintptr_t)rc + PGSIZE);
            assert(CHUNKS_GET_FLAG_ADD(t) == CHUNKS_PAGE_SLAB);
            spin_lock(&slabs_i[idx].lk);
            list_insert((Chunk*)t);
            t -> cpu = cpu; 
            spin_unlock(&slabs_i[idx].lk);

        }


    }
    return (uintptr_t *)rc;
}

void slabs_free(uintptr_t * pointer) {
    assert(pointer);
    Chunk *chunk = (Chunk *)pointer;
    assert(chunk);

    assert(CHUNKS_GET_FLAG_ADD(chunk) == CHUNKS_PAGE_SLAB);
    assert(CHUNKS_GET_STATUS_ADD(chunk) == CHUNKS_PAGE_INUSE);
    int idx = CHUNKS_GET_IDX_ADD(chunk);
    assert(idx >= 0);
    Chunk *slabs_i = (Chunk*)((uintptr_t)slabs + (uintptr_t)(sizeof(Chunk) * slabs_size * cpu_current()));
    assert(slabs_i);
    spin_lock(&slabs_i[idx].lk);
    list_insert((Chunk*)pointer);
    spin_unlock(&slabs_i[idx].lk);
}