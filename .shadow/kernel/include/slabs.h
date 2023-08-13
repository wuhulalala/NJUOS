#include <am.h>
#include "chunks.h"
#define MINSIZE 1
#define SLABS_CPU(i) sla_b##i
void slabs_init();
uintptr_t *slabs_malloc(size_t n);
void slabs_free(uintptr_t * pointer);
