#include "buddys.h"

void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
uintptr_t try_lock(spinlock_t *lk); 
void buddys_init() {
    buddys = (Chunk*)((uintptr_t)chunks + (uintptr_t)(sizeof(uintptr_t) * chunks_size));
    assert(buddys);
    buddys_size = log_n(MAXSIZE / PGSIZE) + 1;
    assert(buddys_size > 0);
    chunks_base = (uintptr_t*)ROUNDUP((uintptr_t)buddys + (uintptr_t)(buddys_size * sizeof(Chunk)), MAXSIZE);
    for (int i = 0; i < buddys_size; i++) {
        buddys[i].next = buddys[i].prev = &buddys[i];
        #ifdef TEST
        pthread_mutex_init(&buddys[i].lk, NULL);
        #else
        buddys[i].lk = SPIN_INIT();
        #endif
        
    }

    for (uintptr_t iter = (uintptr_t)chunks_base; iter + MAXSIZE < (uintptr_t)heap.end; iter += MAXSIZE) {
        CHUNKS_SET_IDX_ADD(iter, buddys_size - 1);
        CHUNKS_SET_FLAG_ADD(iter, CHUNKS_PAGE_BUDDY);
        CHUNKS_SET_STATUS_ADD(iter, CHUNKS_PAGE_UNUSED);
        assert(iter);
        spin_lock(&buddys[CHUNKS_GET_IDX_ADD(iter)].lk);
        list_insert((Chunk*)iter);
        spin_unlock(&buddys[CHUNKS_GET_IDX_ADD(iter)].lk);
    }
    for (uintptr_t iter = (uintptr_t)chunks_base; iter + MAXSIZE < (uintptr_t)heap.end; iter += MAXSIZE) {
        assert(iter);
        list_remove((Chunk*)iter);
    }
    printf("buddys initial finished \n");
    
}

void list_insert(Chunk* chunk) {
    Chunk *head = NULL;
    assert(chunk);
    switch (CHUNKS_GET_FLAG_ADD(chunk)) {
    case CHUNKS_PAGE_BUDDY :
        head = &buddys[CHUNKS_GET_IDX_ADD(chunk)];
        assert(head);
        break;
    
    case CHUNKS_PAGE_SLAB :
    // to do
        break;
    }
    #ifndef TEST
    panic_on(try_lock(&buddys[CHUNKS_GET_IDX_ADD(chunk)].lk) == UNLOCKED, "Did not get the lock before the insert");
    #endif
    Chunk *next = head -> next;
    head -> next = chunk;
    chunk -> prev = head;
    next -> prev = chunk;
    chunk -> next = next;

    int count = 0;
    for (Chunk *p = head -> next; p != head; p = p -> next, count ++) {
        printf("this is the %d node of the list, the address is %p\n", count, (void *)p);
    }
    printf("------------------------------------\n");
    assert(head);
}

void list_remove(Chunk *chunk) {
    #ifndef TEST
    panic_on(try_lock(&buddys[CHUNKS_GET_IDX_ADD(chunk)].lk) == UNLOCKED, "Did not get the lock before the remove");
    #endif
    assert(chunk -> next != chunk);
    Chunk* head = &buddys[CHUNKS_GET_IDX_ADD(chunk)];
    Chunk *next = chunk -> next, *prev = chunk -> prev;
    prev -> next = next;
    next -> prev = prev;
    assert(prev -> next);
    assert(next -> prev);
    int count = 0;
    for (Chunk *p = head -> next; p != head; p = p -> next, count ++) {
        printf("this is the %d node of the list, the address is %p\n", count, (void *)p);
    }
    printf("------------------------------------\n");
}

uintptr_t *buddys_malloc(size_t n) {
    size_t exponent = log_n(n);
    size_t actual_size = mem_request2_size(n);
    assert(actual_size);
    int idx = exponent - 12;
    int baseline = idx;
    assert(idx >= 0);
    Chunk *head = NULL, *pointer = NULL;
    while (idx < buddys_size) {
        head = &buddys[idx];
        if (head != head -> next) break;
        idx++;
    }
    if (idx == buddys_size) {
        return NULL;
    }
    spin_lock(&buddys[idx].lk);
    pointer = buddys[idx].next;
    assert(pointer);
    list_remove(pointer); 
    spin_unlock(&buddys[idx].lk);
    assert(CHUNKS_GET_FLAG_ADD((uintptr_t)pointer) != CHUNKS_PAGE_SLAB);
    assert(CHUNKS_GET_STATUS_ADD((uintptr_t)pointer) != CHUNKS_PAGE_INUSE);

    CHUNKS_SET_IDX_ADD(pointer, baseline);
    CHUNKS_SET_FLAG_ADD(pointer, CHUNKS_PAGE_BUDDY);
    CHUNKS_SET_STATUS_ADD(pointer, CHUNKS_PAGE_INUSE);

    assert(CHUNKS_GET_FLAG_ADD((uintptr_t)pointer) == CHUNKS_PAGE_BUDDY);
    assert(CHUNKS_GET_STATUS_ADD((uintptr_t)pointer) == CHUNKS_PAGE_INUSE);

    Chunk *temp = NULL;
    while (idx > baseline) {
        temp = (Chunk *)((uintptr_t)pointer +  (uintptr_t)(((uintptr_t)1 << baseline) * PGSIZE));
        assert(temp);
        CHUNKS_SET_IDX_ADD(temp, baseline);
        CHUNKS_SET_FLAG_ADD(temp, CHUNKS_PAGE_BUDDY);
        CHUNKS_SET_STATUS_ADD(temp, CHUNKS_PAGE_UNUSED);
        spin_lock(&buddys[baseline].lk);
        list_insert(temp);
        spin_unlock(&buddys[baseline].lk);
        assert(CHUNKS_GET_IDX_ADD(temp) == baseline);
        assert(CHUNKS_GET_FLAG_ADD(temp) == CHUNKS_PAGE_BUDDY);
        assert(CHUNKS_GET_IDX_ADD(temp) == CHUNKS_PAGE_UNUSED);
        baseline++;

    } 
    assert(pointer);
    return (uintptr_t*)pointer;
}
