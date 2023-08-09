#include <common.h>
extern int sum;
static void os_init() {
  pmm->init();
}

static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  //for (int i = 0; i < 500000; i ++) {
    //sum++;
  //} 
  //printf("sum is %d\n", sum);
  while(1);
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
