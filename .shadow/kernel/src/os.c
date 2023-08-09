#include <common.h>

static void os_init() {
  pmm->init();
}

int sum = 0;
static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  while (1) {
    for (int i = 0; i < 50000; i ++) {
      sum++;
    } 
    printf("sum is %d\n", sum);
  };
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
