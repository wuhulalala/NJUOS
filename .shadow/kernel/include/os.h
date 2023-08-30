#ifndef HEADER_FILE_OS  
#define HEADER_FILE_OS  
#include <common.h>

#define NAME_LEN 128
#define KMT_UNLOCK 0
#define KMT_LOCK 1
#define MAGIC_SEQ -1
#define MAGIC_EVENT -1
#define MAGIC_HANDLER NULL

#define KMT_FENCE (0x17377199)
#define KMT_FENCE_SIZE 2
#define KMT_STACK_SIZE (8192)
#define KMT_INIT_ROUND 10
#define uaf_check(ptr) \
  panic_on(KMT_FENCE == *(uint32_t *)(ptr), "use-after-free");


#define TIME_SEQ 0
#define YIELD_SEQ 1

struct task {
  char name[NAME_LEN];
  enum {
    READY,
    RUNNING,
    WAIT_TO_SCHEDULE,
    WAIT_TO_LOAD,
    WAIT_TO_WAKE,
    WAIT_TO_WAKE_AND_SCHEDULE,
    DEAD
  } status;
  int round;
  Context *context;
  task_t *next, *prev;
  uint32_t fence1[KMT_FENCE_SIZE];
  uint8_t stack[KMT_STACK_SIZE];
  uint32_t fence2[KMT_FENCE_SIZE];

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
  char name[NAME_LEN];
  spinlock_t lock;
  int count; 
  task_t *wait_list;
};

typedef struct irq {
  int seq;
  int event;
  handler_t handler;
  struct irq *next;
}IRQ;

task_t **current_task;
Context **schedule_context;
task_t *tasks;
spinlock_t irq_lk;
spinlock_t task_lk;
task_t task_head;
#endif