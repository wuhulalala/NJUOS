#include "thread.h"
extern mod_pmm_t *pmm;
extern Area heap;
static void entry(int tid) { pmm->alloc(128); }
static void goodbye()      { printf("End.\n"); }
int main() {
    pmm->init();
    for (int i = 0; i < 4; i++)
    create(entry);
    join(goodbye);
}
