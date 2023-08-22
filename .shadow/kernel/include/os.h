#include <common.h>
#define NAME_LEN 128
#define KMT_UNLOCK 0
#define KMT_LOCK 1
struct task {
  // TODO
};

struct spinlock {
  // TODO
  char name[NAME_LEN];
  bool saved_interrupt_status;
  int lock;
};

struct semaphore {
  // TODO
};