#ifndef HEADER_FILE_BUDDY  
#define HEADER_FILE_BUDDY  
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#include <am.h>
#include "chunks.h"
void buddys_init();
void list_insert(Chunk* chunk);

void list_remove(Chunk *chunk); 

uintptr_t *buddys_malloc(size_t n);
void buddys_free(uintptr_t *pointer);

#endif 