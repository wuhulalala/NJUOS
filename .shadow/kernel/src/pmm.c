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
  chunks_size = (((uintptr_t)heap.end - (uintptr_t)heap.start) + PGSIZE - 1) / PGSIZE;
  printf("the chunks is [%p, %p), the chunks size is %d\n", (uintptr_t)chunks, (uintptr_t)(chunks) + (uintptr_t)(chunks_size) * PGSIZE, chunks_size); 

}
#else
// 测试代码的 pmm_init ()
static void pmm_init() {
  char *ptr  = malloc(HEAP_SIZE);
  heap.start = ptr;
  heap.end = ptr + HEAP_SIZE;
  printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
}
#endif
MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
