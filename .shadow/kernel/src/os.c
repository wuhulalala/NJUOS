#include <common.h>
extern int sum;
static void os_init() {
  pmm -> init();
  //kmt -> init();
}

static void os_run() {
  while (1) {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  yield();
  }
}

static Context * os_trap(Event ev, Context *context) {

  printf("cpu %d : Hello World\n", cpu_current());
  return context;
}
static void os_on_irq(int seq, int event, handler_t handler) {

}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap  = os_trap,
  .on_irq  = os_on_irq,
};
