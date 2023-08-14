#include "thread.h"
#include "kernel.h"
#include <stdlib.h>
#include <time.h>
#define PGSIZE 4096
#define MINSIZE 64
#define NTHREAD 4
static void entry1(int id) { 
    srand(time(NULL));
    for (int i = 0; i < 1000000; i++) {
        int random = rand() % 4095 + 1;
        int random_mem = (random) * PGSIZE + random;
        char *mem = pmm->alloc(random_mem); 
        printf("thread %d malloc success\n", id);
        assert(mem);
        pmm->free(mem); 
        printf("thread %d free success\n", id);

    }
}

static void entry2(int id) { 
    srand(time(NULL));
    for (int i = 0; i < 1000000; i++) {
        int random = rand() % 16 + 1;
        int random_mem = (random) * PGSIZE + random;
        char *mem = pmm->alloc(random_mem); 
        printf("thread %d malloc success\n", id);
        assert(mem);
        pmm->free(mem); 
        printf("thread %d free success\n", id);

    }
}

static void goodbye() { 
    printf("goodbye.\n"); 
}

int main(int argc, char *argv[]) {
    if (argc < 2) exit(1);
    pmm->init();
    #ifdef BUDDY
    switch(atoi(argv[1])) {
        case 0: do_buddy_test_0();
                break;
        case 1: do_buddy_test_1();
                break;
        case 2: do_buddy_test_2();
                break;
        case 3: do_buddy_test_3();
                break;
    }
    #endif
    #ifdef SLAB
    switch(atoi(argv[1])) {
        case 0: do_slab_test_0();
                break;
        case 1: do_slab_test_1();
                break;
        case 2: do_slab_test_2();
                break;
        case 3: do_slab_test_3();
                break;
    }
    #endif
}
void do_buddy_test_0() {
    char *temp = pmm -> alloc(PGSIZE);

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
    pmm -> free(temp);
    printf("test 0 finished\n");
    //for (int i = 0; i < 4; i++)
    //create(entry);
    //join(goodbye);
}
void do_buddy_test_1() {

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

void do_buddy_test_2() {

    for (int t = 0; t < NTHREAD; t++) {
        create(entry1);
    }
    join(goodbye);
}

void do_buddy_test_3() {

    for (int t = 0; t < NTHREAD; t++) {
        create(entry2);
    }
    join(goodbye);
}

void do_slab_test_0() {
    srand(time(NULL));
    char *mem = pmm -> alloc(128);
    pmm -> free(mem); 
    printf("End\n");
    //for (int i = 0; i < 1000000; i++) {
        //int random = rand() % (PGSIZE - MINSIZE) + MINSIZE;
        //printf("malloc %d bytes memory\n", random);
        //char *mem = pmm->alloc(random); 
        //if (!mem) {
            //printf("the random_mem is %d\n", random);
        //}
        //pmm->free(mem); 
        ////printf("free %d page memory\n", random_mem / 4096);
        //printf("finished %d round\n", i + 1);

    //}
}

void do_slab_test_1() {
    
}

void do_slab_test_2() {
    
}

void do_slab_test_3() {
    
}