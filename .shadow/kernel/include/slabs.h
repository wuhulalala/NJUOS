#ifndef HEADER_FILE_SLAB  
#define HEADER_FILE_SLAB  
#include <am.h>
#include "chunks.h"
#ifdef TEST
#define _GNU_SOURCE
//#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#define cpu_count() sysconf(_SC_NPROCESSORS_ONLN)
#define cpu_current() sched_getcpu()
#endif

#define MINSIZE sizeof(Chunk)
#define SLABS_CPU(i) sla_b##i
void slabs_init();
uintptr_t *slabs_malloc(size_t n);
void slabs_free(uintptr_t * pointer);
#endif