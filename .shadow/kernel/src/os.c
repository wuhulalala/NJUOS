#include <common.h>
extern int sum;
static void os_init() {
  pmm -> init();
  kmt -> init();
}

static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  //for (int i = 0; i < 500000; i ++) {
    //sum++;
  //} 
  //printf("sum is %d\n", sum);
  printf("the cpu count is %d\n", cpu_count());
  //pmm -> alloc(5 * 4096);
  while(1);
}

static Context * os_trap(Event ev, Context *context) {
  printf("Hello World\n");
  return NULL;
}
static void os_on_irq(int seq, int event, handler_t handler) {

}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap  = os_trap,
  .on_irq  = os_on_irq,
};
