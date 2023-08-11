#ifndef HEADER_FILE_H  
#define HEADER_FILE_H  
#include <am.h>
#include "spinlock.h"
#include "chunks.h"
void buddys_init();
void list_insert(Chunk* chunk);

void list_remove(Chunk *chunk); 

#endif 