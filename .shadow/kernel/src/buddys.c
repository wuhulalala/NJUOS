#include "buddys.h"
#include "chunks.h"
void buddys_init() {
    buddys = (Chunk*)((uintptr_t)chunks + (uintptr_t)(sizeof(uintptr_t) * chunks_size));
    assert(buddys);
    buddys_size = log_n(MAXSIZE / PGSIZE) + 1;
    assert(buddys_size > 0);
    chunks_base = (uintptr_t*)ROUNDDOWN((uintptr_t)buddys + (uintptr_t)(buddys_size * sizeof(Chunk)), MAXSIZE);
    for (int i = 0; i < buddys_size; i++) {
        buddys[i].next = buddys[i].prev = &buddys[i];
        buddys[i].lk = SPIN_INIT();
    }

    for (uintptr_t iter = (uintptr_t)chunks_base; iter < heap.end; iter += MAXSIZE) {
        CHUNKS_SETIDX_ADD(iter, buddys_size - 1);
        CHUNKS_SETFLAG_ADD(iter, CHUNKS_PAGE_BUDDY);
        CHUNKS_SETSTATUS_ADD(iter, CHUNKS_PAGE_UNUSED);
        assert(iter);
        list_insert((Chunk*)iter);
        printf("the index is %p, the flag is %p, the status is %p\n", CHUNKS_GETIDX_ADD(chunks + 10), 
                CHUNKS_GETFLAG_ADD(iter), CHUNKS_GETSTATUS_ADD(iter));
    }
    printf("buddys initial finished \n");
    
}

void list_insert(Chunk* chunk) {
    Chunk *head = NULL;
    assert(chunk);
    switch (CHUNKS_GETFLAG_ADD(chunk)) {
    case CHUNKS_PAGE_BUDDY :
        head = &buddys[CHUNKS_GETIDX_ADD(chunk)];
        assert(head);
        break;
    
    case CHUNKS_PAGE_SLAB :
    // to do
        break;
    }
    Chunk *next = head -> next;
    spin_lock(buddys[CHUNKS_GETIDX_ADD(chunk)].lk);
    head -> next = chunk;
    chunk -> prev = head;
    next -> prev = chunk;
    chunk -> next = next;
    spin_unlock(buddys[CHUNKS_GETIDX_ADD(chunk)].lk);
    assert(head);
}

void list_remove(Chunk *chunk) {
    assert(chunk -> next != chunks);
    Chunk *next = chunk -> next, *prev = chunk -> prev;
    spin_lock(buddys[CHUNKS_GETIDX_ADD(chunk)].lk);
    prev -> next = next;
    next -> prev = prev;
    spin_unlock(buddys[CHUNKS_GETIDX_ADD(chunk)].lk);
    assert(prev -> next);
    assert(next -> prev);
}