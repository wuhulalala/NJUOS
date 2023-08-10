#ifndef TEST
#include <common.h>
#include "spinlock.h"
#include "chunks.h"
spinlock_t lk = SPIN_INIT();
#endif
#ifdef TEST
#define HEAP_SIZE 128 * (1 << 20)
typedef struct {
  void *start, *end;
} Area;
Area heap = {};
#include <stddef.h>
#include "kernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "chunks.h"
#include <assert.h>
#endif

uintptr_t *chunks = NULL;
uintptr_t chunks_size = 0;

static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

#ifndef TEST
// 框架代码中的 pmm_init (在 AbstractMachine 中运行)
static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  chunks = (uintptr_t*)heap.start;
  assert(chunks);
  chunks_size = (((uintptr_t)heap.end - (uintptr_t)heap.start) + PGSIZE - 1) / PGSIZE;
  printf("the chunks is [%p, %p), the chunks size is %d\n", (uintptr_t)chunks, (uintptr_t)(chunks) + (uintptr_t)(chunks_size) * PGSIZE, chunks_size); 

}
#else
 // 测代码的 pmm_init ()
static void pmm_init() {
  char *ptr  = malloc(HEAP_SIZE);
  heap.start = ptr;
  heap.end = ptr + HEAP_SIZE;
  printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
  chunks = (uintptr_t*)heap.start;
  assert(chunks);
  chunks_size = (((uintptr_t)heap.end - (uintptr_t)heap.start) + PGSIZE - 1) / PGSIZE;
  printf("the address is in the %d page\n", ((uintptr_t)ROUNDUP(heap.start + PGSIZE, PGSIZE) - (uintptr_t)heap.start) / PGSIZE);
  chunks[1] = 0xfffffffffffffff0;
  chunks[0] = 0x0000000000000000;
  printf("the chunk is %p\n", chunks[1]);
  printf("the idx mask is %p, the flag mask is %p, the status flag is %p\n", CHUNKS_IDX_MASK, CHUNKS_FLAG_MASK, CHUNKS_STATUS_MASK);
  printf("idx is %p, flag is %p, status is %p\n", CHUNKS_GET_IDX(chunks[0]), CHUNKS_GET_FLAG(chunks[0]), CHUNKS_GET_STATUS(chunks[0]));

  printf("the index is %p, the flag is %p, the status is %p\n", CHUNKS_GETIDX_ADD(chunks + 10), CHUNKS_GETFLAG_ADD(chunks + 10), CHUNKS_GETSTATUS_ADD(chunks + 10));
  CHUNKS_SETIDX_ADD((uintptr_t)chunks + (uintptr_t)10, 100);
  CHUNKS_SETFLAG_ADD((uintptr_t)chunks + (uintptr_t)10, 1);
  CHUNKS_SETSTATUS_ADD((uintptr_t)chunks + (uintptr_t)10, 1);
  printf("the chunks0 is %p\n", chunks[0]);
  printf("the index is %ld, the flag is %ld, the status is %ld\n", CHUNKS_GETIDX_ADD(chunks + 10), CHUNKS_GETFLAG_ADD(chunks + 10), CHUNKS_GETSTATUS_ADD(chunks + 10));
  printf("the chunks is [%p, %p), the chunks size is %d\n", (uintptr_t)chunks, (uintptr_t)(chunks) + (uintptr_t)(chunks_size) * PGSIZE, chunks_size); 
}
#endif
MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
