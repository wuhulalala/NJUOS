#include "spinlock.h"
#ifndef TEST
#include <klib-macros.h>
#else
#define ROUNDUP(a, sz)      ((((uintptr_t)a) + (sz) - 1) & ~((sz) - 1))
#define ROUNDDOWN(a, sz)    ((((uintptr_t)a)) & ~((sz) - 1))
#define LENGTH(arr)         (sizeof(arr) / sizeof((arr)[0]))
#define RANGE(st, ed)       (Area) { .start = (void *)(st), .end = (void *)(ed) }
#define IN_RANGE(ptr, area) ((area).start <= (ptr) && (ptr) < (area).end)
#define putstr(s) \
  ({ for (const char *p = s; *p; p++) putch(*p); })
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "buddys.h"
#include <assert.h>
#include "kernel.h"
#define HEAP_SIZE 128 * (1 << 20)
typedef struct {
  void *start, *end;
} Area;
Area heap;
#endif
#define PGSIZE 4096
#define MAXSIZE (16 << 20)

typedef struct Chunk {
    struct Chunk *prev;
    struct Chunk *next; 
    spinlock_t lk; 
}Chunk;

uintptr_t *chunks;
size_t chunks_size;

uintptr_t *chunks_base;
Chunk *buddys;
size_t buddys_size;
#define SPIN_INIT() 0
// some helpful marcos
#define CHUNKS_STATUS_SIZE (1)
#define CHUNKS_FLAG_SIZE (1)
#define CHUNKS_IDX_SIZE (sizeof(uintptr_t) * 8 - CHUNKS_STATUS_SIZE - CHUNKS_FLAG_SIZE)

#define CHUNKS_PAGE_UNUSED (0)
#define CHUNKS_PAGE_INUSE (1)

#define CHUNKS_PAGE_BUDDY (1)
#define CHUNKS_PAGE_SLAB (0)

#define CHUNKS_IDX_MASK ((uintptr_t)0xffffffffffff >> (CHUNKS_STATUS_SIZE + CHUNKS_FLAG_SIZE))
#define CHUNKS_FLAG_MASK ((uintptr_t)1 << CHUNKS_IDX_SIZE + CHUNKS_STATUS_SIZE)
#define CHUNKS_STATUS_MASK ((uintptr_t)1 << CHUNKS_IDX_SIZE)

#define CHUNKS_GET_IDX(val) ((uintptr_t)(val) & CHUNKS_IDX_MASK)
#define CHUNKS_GET_FLAG(val) (((uintptr_t)(val) & CHUNKS_FLAG_MASK) >> (CHUNKS_IDX_SIZE + CHUNKS_STATUS_SIZE))
#define CHUNKS_GET_STATUS(val) (((uintptr_t)(val) & CHUNKS_STATUS_MASK) >> CHUNKS_IDX_SIZE)

#define CHUNKS_SET_IDX(val, new_value) ((((val) >> CHUNKS_IDX_SIZE) << CHUNKS_IDX_SIZE) | (uintptr_t)(new_value))
#define CHUNKS_SET_FLAG(val, new_value) ((((val) << 1) >> 1) | (uintptr_t)(new_value) << (CHUNKS_IDX_SIZE + CHUNKS_STATUS_SIZE))
#define CHUNKS_SET_STATUS(val, new_value) ((((((val) << 1) >> 1) >> CHUNKS_IDX_SIZE) == (uintptr_t)(new_value)) ? (val) : (val) ^ CHUNKS_STATUS_MASK)

#define CHUNKS_GET_IDX_ADD(add) CHUNKS_GET_IDX(chunks[((uintptr_t)ROUNDUP(add, PGSIZE) - (uintptr_t)heap.start) / PGSIZE])
#define CHUNKS_GET_FLAG_ADD(add) CHUNKS_GET_FLAG(chunks[((uintptr_t)ROUNDUP(add, PGSIZE) - (uintptr_t)heap.start) / PGSIZE])
#define CHUNKS_GET_STATUS_ADD(add) CHUNKS_GET_STATUS(chunks[((uintptr_t)ROUNDUP(add, PGSIZE) - (uintptr_t)heap.start) / PGSIZE])

#define CHUNKS_SET_IDX_ADD(add, val) CHUNKS_SET_IDX(chunks[((uintptr_t)ROUNDUP(add, PGSIZE) - (uintptr_t)heap.start) / PGSIZE], val)
#define CHUNKS_SET_FLAG_ADD(add, val) CHUNKS_SET_FLAG(chunks[((uintptr_t)ROUNDUP(add, PGSIZE) - (uintptr_t)heap.start) / PGSIZE], val)
#define CHUNKS_SET_STATUS_ADD(add, val) CHUNKS_SET_STATUS(chunks[((uintptr_t)ROUNDUP(add, PGSIZE) - (uintptr_t)heap.start) / PGSIZE], val)
