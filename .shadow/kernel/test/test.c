#include "thread.h"
#include "kernel.h"
static void entry(int tid) { pmm->alloc(5 * PGSIZE); }
static void goodbye()      { printf("End.\n"); }
int main() {
    pmm->init();
    pmm->alloc(5 * PGSIZE);
    printf("End\n");
    //for (int i = 0; i < 4; i++)
    //create(entry);
    //join(goodbye);
}
