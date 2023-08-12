#include "thread.h"
#include "kernel.h"
#include <stdlib.h>
#include <time.h>
#define PGSIZE 4096
static void entry(int tid) { 
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        int random = rand() % 4096 + 1;
        int random_mem = (random - 1) * PGSIZE + random;
        char *mem = pmm->alloc(random_mem); 
        pmm->free(mem); 

    }
}
static void goodbye() { 
    printf("End.\n"); 
}
int main() {
    pmm->init();
    printf("End\n");
    for (int i = 0; i < 4; i++)
    create(entry);
    join(goodbye);
}
