#include "buddys.h"

void buddys_init() {
    buddys = (Chunk*)((uintptr_t)chunks + (uintptr_t)(sizeof(uintptr_t) * chunks_size));
    assert(buddys);
    buddys_size = log_n(MAXSIZE / PGSIZE) + 1;
    assert(buddys_size > 0);
    chunks_base = (uintptr_t*)ROUNDUP((uintptr_t)buddys + (uintptr_t)(buddys_size * sizeof(Chunk)), MAXSIZE);
    for (int i = 0; i < buddys_size; i++) {
        buddys[i].next = buddys[i].prev = &buddys[i];
        buddys[i].lk = SPIN_INIT();
    }

    for (uintptr_t iter = (uintptr_t)chunks_base; iter + MAXSIZE < (uintptr_t)heap.end; iter += MAXSIZE) {
        CHUNKS_SET_IDX_ADD(iter, buddys_size - 1);
        CHUNKS_SET_FLAG_ADD(iter, CHUNKS_PAGE_BUDDY);
        CHUNKS_SET_STATUS_ADD(iter, CHUNKS_PAGE_UNUSED);
        assert(iter);
        list_insert((Chunk*)iter);
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
    Chunk *next = head -> next;
    spin_lock(&buddys[CHUNKS_GET_IDX_ADD(chunk)].lk);
    head -> next = chunk;
    chunk -> prev = head;
    next -> prev = chunk;
    chunk -> next = next;
    spin_unlock(&buddys[CHUNKS_GET_IDX_ADD(chunk)].lk);

    int count = 0;
    for (Chunk *p = head -> next; p != head; p = p -> next, count ++) {
        printf("this is the %d node of the list, the address is %p\n", count, (void *)p);
    }
    printf("------------------------------------\n");
    assert(head);
}

void list_remove(Chunk *chunk) {
    assert(chunk -> next != chunk);
    Chunk *next = chunk -> next, *prev = chunk -> prev;
    spin_lock(&buddys[CHUNKS_GET_IDX_ADD(chunk)].lk);
    prev -> next = next;
    next -> prev = prev;
    spin_unlock(&buddys[CHUNKS_GET_IDX_ADD(chunk)].lk);
    assert(prev -> next);
    assert(next -> prev);
    int count = 0;
    for (Chunk *p = chunk -> next; p != chunk; p = p -> next, count ++) {
        printf("this is the %d node of the list, the address is %p\n", count, (void *)p);
    }
    printf("------------------------------------\n");
}