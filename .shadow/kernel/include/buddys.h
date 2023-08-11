#ifndef HEADER_FILE_H  
#define HEADER_FILE_H  
#include <am.h>
#include "chunks.h"
#include "spinlock.h"
void buddys_init();
void list_insert(Chunk* chunk);

void list_remove(Chunk *chunk); 

uintptr_t *buddys_malloc(size_t n);

#endif 