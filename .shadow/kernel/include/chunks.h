#include "spinlock.h"
#define PGSIZE 4096
#define CHUNK_BASE 
#define MAXSIZE (16 << 20)
typedef struct Chunk {
    struct Chunk *prev;
    struct Chunk *next;
}Chunk;

typedef struct Buddy {
    Chunk *prev;
    Chunk *next; 
    spinlock_t lk; 
}Buddy;

