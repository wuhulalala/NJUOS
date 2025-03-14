#ifndef TEST
#include <common.h>
#include "chunks.h"
#endif
#ifdef TEST
#include "chunks.h"
Area heap = {};
#endif
// some global variables defined in the chunks.h
uintptr_t *chunks = NULL;
size_t chunks_size = 0;

uintptr_t *chunks_base = NULL;
Chunk *buddys = NULL;
size_t buddys_size = 0;

// buddys function
void buddys_init();
uintptr_t *buddys_malloc(size_t n);
void buddys_free(uintptr_t * pointer);

// slabs function
void slabs_init();
uintptr_t *slabs_malloc(size_t n);
void slabs_free(uintptr_t * pointer);



static void *kalloc(size_t size) {
  uintptr_t *pointer = NULL;
  size_t actual_size = mem_request2_size(size);
  if (actual_size <= MINSIZE) actual_size = MINSIZE;
  if (actual_size >= PGSIZE) {
    pointer = buddys_malloc(actual_size);
    assert(pointer);
  } else {
    pointer = slabs_malloc(actual_size);
    assert(pointer);
  }
  //assert(pointer);
  return (void*)pointer;
}

static void kfree(void *ptr) {
  assert(ptr);
  if (CHUNKS_GET_FLAG_ADD(ptr) == CHUNKS_PAGE_SLAB) {
    slabs_free(ptr);
  } else if (CHUNKS_GET_FLAG_ADD(ptr) == CHUNKS_PAGE_BUDDY){
    buddys_free((uintptr_t*)ptr);
  } else {
    printf("free fail, wrong address\n");
  }
}

static void *kalloc_safe(size_t size) {
  bool i = ienabled();
  iset(false);
  void *ret = kalloc(size);
  if (i) iset(true);
  return ret;
}

static void kfree_safe(void *ptr) {
  int i = ienabled();
  iset(false);
  kfree(ptr);
  if (i) iset(true);
}

#ifndef TEST
// 框架代码中的 pmm_init (在 AbstractMachine 中运行)
static void pmm_init() {
  //uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  //printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  chunks = (uintptr_t*)heap.start;
  assert(chunks);
  chunks_size = (((uintptr_t)heap.end - (uintptr_t)heap.start) + PGSIZE - 1) / PGSIZE;
  //printf("the chunks is [%p, %p), the chunks size is %d\n", (uintptr_t)chunks, (uintptr_t)(chunks) + (uintptr_t)(chunks_size) * PGSIZE, chunks_size); 
  buddys_init();
  slabs_init();
}
#else
 // 测代码的 pmm_init ()
static void pmm_init() {
  char *ptr  = malloc(HEAP_SIZE);
  heap.start = ptr;
  heap.end = ptr + HEAP_SIZE;
  //printf("Got %d MiB heap: [%p, %p)\n", HEAP_SIZE >> 20, heap.start, heap.end);
  chunks = (uintptr_t*)heap.start;
  assert(chunks);
  chunks_size = (((uintptr_t)heap.end - (uintptr_t)heap.start) + PGSIZE - 1) / PGSIZE;


  //printf("the index is %p, the flag is %p, the status is %p\n", CHUNKS_GETIDX_ADD(chunks + 10), CHUNKS_GETFLAG_ADD(chunks + 10), CHUNKS_GETSTATUS_ADD(chunks + 10));
  //chunks[0] = CHUNKS_SET_IDX(chunks[0], 100);
  //chunks[0] = CHUNKS_SET_FLAG(chunks[0], 1);
  //chunks[0] = CHUNKS_SET_STATUS(chunks[0], 1);
  //printf("the chunks0 is %p\n", chunks[0]);
  //printf("the index is %p, the flag is %ld, the status is %ld\n", CHUNKS_GETIDX_ADD(chunks + 10), CHUNKS_GETFLAG_ADD(chunks + 10), CHUNKS_GETSTATUS_ADD(chunks + 10));
  //printf("the chunks is [%p, %p), the chunks size is %d\n", (uintptr_t)chunks, (uintptr_t)(chunks) + (uintptr_t)(chunks_size) * PGSIZE, chunks_size); 
  buddys_init();
  slabs_init();
}
#endif
MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc_safe,
  .free  = kfree_safe,
};
