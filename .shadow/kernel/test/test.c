#include "thread.h"
#include "am.h"
extern mod_pmm_t *pmm;
static void entry(int tid) { pmm->alloc(128); }
static void goodbye()      { printf("End.\n"); }
int main() {
    pmm->init();
    for (int i = 0; i < 4; i++)
    create(entry);
    join(goodbye);
}
