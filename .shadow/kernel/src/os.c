#include <os.h>
#include <common.h>
#include <devices.h>
extern spinlock_t irq_lk;
void kmt_schedule();

task_t *task_alloc();


static void tty_reader(void *arg) {
  device_t *tty = dev->lookup(arg);
  char cmd[128], resp[128], ps[16];
  snprintf(ps, 16, "(%s) $ ", arg);
  while (1) {
    tty->ops->write(tty, 0, ps, strlen(ps));
    int nread = tty->ops->read(tty, 0, cmd, sizeof(cmd) - 1);
    cmd[nread] = '\0';
    sprintf(resp, "tty reader task: got %d character(s).\n", strlen(cmd));
    tty->ops->write(tty, 0, resp, strlen(resp));
  }
}

static void os_init() {
  pmm -> init();
  kmt -> init();
  kmt->create(task_alloc(), "tty_reader", tty_reader, "tty1");
  kmt->create(task_alloc(), "tty_reader", tty_reader, "tty2");
}

static IRQ irq = {
  .seq = MAGIC_SEQ,
  .handler = MAGIC_HANDLER,
  .event = MAGIC_EVENT,
  .next = NULL,
};

static void os_run() {
  kmt_schedule();
}

static Context * os_trap(Event ev, Context *context) {
  panic_on(ienabled() != false, "interrupt do not closed");
  Context *next = NULL;
  for (IRQ *h = irq.next; h != NULL; h = h -> next) {
    if (h -> event == EVENT_NULL || h -> event == ev.event) {
      Context *r = h -> handler(ev, context);
      panic_on(r && next, "returning multiple contexts");
      if (r) next = r;
    }
  }
  panic_on(!next, "returning NULL context");
  //panic_on(sane_context(next), "returning to invalid context");
  return next;
}

static void os_on_irq(int seq, int event, handler_t handler) {
  IRQ * node = (IRQ*)pmm -> alloc(sizeof(IRQ));
  assert(node);
  assert(irq.event == MAGIC_EVENT);
  assert(irq.seq == MAGIC_SEQ);
  assert(irq.handler == MAGIC_HANDLER);

  node -> seq = seq;
  node -> event = event;
  node -> handler = handler;
  node -> next = NULL;

  IRQ *p = NULL;
  kmt -> spin_lock(&irq_lk);
  for (p = &irq; p -> next != NULL && (p -> next) -> seq < seq; p = p -> next);
  assert(p);
  node -> next = p -> next;
  p -> next = node;
  kmt -> spin_unlock(&irq_lk);
}


MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap  = os_trap,
  .on_irq  = os_on_irq,
};