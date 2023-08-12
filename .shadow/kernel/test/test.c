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
        assert(mem);
        pmm->free(mem); 

    }
}
static void goodbye() { 
    printf("End.\n"); 
}
int main(int argc, char *argv[]) {
    if (argc < 2) exit(1);
    pmm->init();
    switch(atoi(argv[1])) {
        case 0: do_test_0();
        case 1: do_test_1();
}
}
void do_test_0() {
    srand(time(NULL));
    for (int i = 0; i < 1000000; i++) {
        int random = rand() % 4095 + 1;
        int random_mem = (random) * PGSIZE + random;
        printf("malloc %d page memory\n", random_mem / 4096);
        char *mem = pmm->alloc(random_mem); 
        if (!mem) {
            printf("the random_mem is %d\n", random_mem);
        }
        pmm->free(mem); 
        //printf("free %d page memory\n", random_mem / 4096);
        printf("finished %d round\n", i + 1);

    }
    printf("test 0 finished\n");
    //for (int i = 0; i < 4; i++)
    //create(entry);
    //join(goodbye);
}
void do_test_1() {

    srand(time(NULL));
    for (int i = 0; i < 1000000; i++) {
        int random = rand() % 16 + 1;
        int random_mem = (random) * PGSIZE + random;
        printf("malloc %d page memory\n", random_mem / 4096);
        char *mem = pmm->alloc(random_mem); 
        if (!mem) {
            printf("the random_mem is %d\n", random_mem);
        }
        pmm->free(mem); 
        //printf("free %d page memory\n", random_mem / 4096);
        printf("finished %d round\n", i + 1);

    }
    printf("End\n");
}