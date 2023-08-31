#include <common.h>
#include <os.h>
#include <devices.h>

static void list_add(task_t *head, task_t *task);
static void list_insert(task_t *head, task_t *task) {
    panic_on(atomic_xchg(&(task_lk.lock), KMT_LOCK) == KMT_UNLOCK, "error, the lock is not acquired");
    assert(head);
    assert(task);
    head -> next -> prev = task;
    task -> next = head -> next;
    task -> prev = head;
    head -> next = task;

}

//static void list_remove(task_t *head, task_t *task) {
    //panic_on(atomic_xchg(&(task_lk.lock), KMT_LOCK) == KMT_UNLOCK, "error, the lock is not acquired");
    //assert(head);
    //assert(task);
    //task_t *p = NULL;
    //for (p = head; p != task; p = p -> next);
    //task_t *prev = p -> prev;
    //prev -> next = p -> next;
    //p -> next -> prev = prev;
    //pmm -> free(task);

//}
static void idle_entry(void *arg) {
  //printf("Hello World\n");
  //device_t *tty = dev->lookup(arg);
  //char cmd[128], resp[128], ps[16];
  //snprintf(ps, 16, "(%s) $ ", arg);
  //while (1) {
    //tty->ops->write(tty, 0, ps, strlen(ps));
    //int nread = tty->ops->read(tty, 0, cmd, sizeof(cmd) - 1);
    //cmd[nread] = '\0';
    //sprintf(resp, "tty reader task: got %d character(s).\n", strlen(cmd));
    //tty->ops->write(tty, 0, resp, strlen(resp));
  //}
  while (1) {
    //printf("cpu count is %d\n", cpu_count());
    //printf("Hello from cpu %d\n", cpu_current());
    yield();
  }
}

static void check_static_fence(task_t *task);

static void kmt_spin_init(spinlock_t *lk, const char *name) {
    strcpy(lk -> name, name);
    lk -> lock = KMT_UNLOCK; }

static void kmt_spin_lock(spinlock_t *lk) {
    bool interrupt_status = ienabled();
    iset(false);

    while (atomic_xchg(&(lk -> lock), KMT_LOCK) == KMT_LOCK);

    lk -> saved_interrupt_status = interrupt_status;
}

static void kmt_spin_unlock(spinlock_t *lk) {
    bool interrupt_status = lk -> saved_interrupt_status;
    panic_on(atomic_xchg(&(lk -> lock), KMT_UNLOCK) == KMT_UNLOCK, "error, does not get the lock");
    iset(interrupt_status);
}



// schedule module
Context *kmt_save_context(Event ev, Context *context) {
    panic_on(!context, "NULL context");
    int cpu = cpu_current();
    task_t *task = current_task[cpu];
    assert(task);
    switch (task -> status)
    {
    case RUNNING:
        // save the context
    
    case WAIT_TO_WAKE_AND_SCHEDULE:

        task -> context = context; 
        break;

    case WAIT_TO_LOAD:

        schedule_context[cpu] = context;
        break;

    default:
        panic("error status");
    }
    return NULL;

}


Context *irq_time_handler(Event ev, Context *context) {
    panic_on(!context, "NULL context");
    int cpu = cpu_current();
    task_t *task = current_task[cpu];
    panic_on(!task, "NULL task");
    panic_on(ev.event != EVENT_IRQ_TIMER, "Not timer interrupt");

    switch (task -> status)
    {
    case RUNNING:

        //printf("hello world\n");
        if (--(task -> round) < 0) 
            task -> status = WAIT_TO_SCHEDULE;

        break;
    
    case WAIT_TO_WAKE_AND_SCHEDULE:
        // before yield the cpu, the timer interrupt happend

        break;

    default:
        panic("error status");
    }
    return NULL;

}


Context *irq_yield_handler(Event ev, Context *context) {
    panic_on(!context, "NULL context");
    int cpu = cpu_current();
    task_t *task = current_task[cpu];
    panic_on(!task, "NULL task");
    panic_on(ev.event != EVENT_YIELD, "Not timer interrupt");

    switch (task -> status)
    {
    case RUNNING:

        task -> status = WAIT_TO_SCHEDULE;

        break;
    
    case WAIT_TO_WAKE_AND_SCHEDULE:

        break;

    case WAIT_TO_LOAD:
        task -> status = RUNNING;
        break;

    default:
        panic("error status");
    }
    return NULL;
}

Context *kmt_load_context(Event en, Context * context) {

    panic_on(!context, "NULL context");
    int cpu = cpu_current();
    task_t *task = current_task[cpu];
    panic_on(!task, "NULL task");

    Context *next = NULL;
    switch (task -> status)
    {
    case RUNNING:
        /* code */
        next = task -> context;
        break;

    case WAIT_TO_WAKE_AND_SCHEDULE:


    case WAIT_TO_SCHEDULE:

        next = schedule_context[cpu];
        break;

    default:
        panic("error status");
    }
    panic_on(!next, "NULL context next");
    check_static_fence(task);
    return next;
}

static void check_static_fence(task_t *task) {
    panic_on(!task, "NULL task");
    for (int i = 0; i < KMT_FENCE_SIZE; i++) {
        panic_on(task -> fence1[i] != KMT_FENCE, "fence1 is broken");
        panic_on(task -> fence2[i] != KMT_FENCE, "fence2 is broken");

    }
}

void kmt_schedule() {
    int cpu = cpu_current();
    while (1) {
        panic_on(ienabled() == true, "interrupt do not closed");
        yield();

        kmt -> spin_lock(&task_lk);
        //printf("i am schedule\n");
        task_t *task = current_task[cpu];
        switch (task -> status)
        {
        case WAIT_TO_SCHEDULE:
            task -> status = READY;
            break;
        
        case WAIT_TO_WAKE_AND_SCHEDULE:
            task -> status = WAIT_TO_WAKE;
            break;
        default:
            printf("task -> status : %d\n", task -> status);
            panic("error status");
        }

        task_t *p = task -> next;
        while (p -> status != READY || p == &task_head) {
            printf("name : %s\n", p -> name);
            printf("status : %d\n", p -> status);
            p = p -> next;
        }
        //printf("======================================================\n");
        p -> round = KMT_INIT_ROUND;
        p -> status = WAIT_TO_LOAD;
        check_static_fence(p);
        

        current_task[cpu] = p;
        kmt -> spin_unlock(&task_lk);
    }
}


task_t *task_alloc() {
    task_t *task = (task_t*)pmm -> alloc(sizeof(task_t));
    panic_on(!task, "there is no space");
    return task;
}

static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {
    if (!task || !entry) {
        return 1;
    }
    strcpy(task -> name, name);
    Area stack = (Area) {&(task -> stack), (&task -> stack) + 1};
    task -> context = kcontext(stack, entry, arg);
    task -> round = KMT_INIT_ROUND;
    task -> status = READY;
    task -> sem_next = task -> sem_prev = task;
    for (int i = 0; i < KMT_FENCE_SIZE; i++) {
        task -> fence1[i] = task -> fence2[i] = KMT_FENCE;
    }
    check_static_fence(task);
    kmt -> spin_lock(&task_lk);
    list_insert(&task_head, task);
    kmt -> spin_unlock(&task_lk);
    check_static_fence(task);
    return 0;
}


static void initialize_task_list(task_t *head, const char *name) {
    assert(head);
    strcpy(head -> name, name);
    head -> context = NULL;
    head -> next = head -> prev = head;
}

static void kmt_init() {
    // initialize the task_list
    initialize_task_list(&task_head, "task list head");

    // initialize the idle thread for each cpu    
    kmt -> spin_init(&irq_lk, "irq queue lock");
    kmt -> spin_init(&task_lk, "tasks lock");
    current_task = (task_t **) pmm -> alloc(sizeof(task_t *) * cpu_count());
    schedule_context = (Context **) pmm -> alloc(sizeof(Context *) * cpu_count());
    panic_on(!current_task, "there is no space");
    panic_on(!schedule_context, "there is no space");

    for (int cpu = 0; cpu < cpu_count(); cpu++) {
        task_t *task = (task_t *)pmm -> alloc(sizeof(task_t));
        panic_on(!task, "there is no space");

        char name[] = "this is the cpu 0";
        name[16] = name[16] + cpu;
        strcpy(task -> name, name);

        Area stack = (Area) {&(task -> stack), &(task -> stack) + 1};
        task -> context = kcontext(stack, idle_entry, NULL);

        task -> round = KMT_INIT_ROUND;

        task -> status = WAIT_TO_LOAD;

        task -> sem_next = task -> sem_prev = task;

        for (int i = 0; i < KMT_FENCE_SIZE; i++) {
            task -> fence1[i] = task -> fence2[i] = KMT_FENCE;
        }

        current_task[cpu] = task;
        check_static_fence(task);
    }

    for (int cpu = 0; cpu < cpu_count(); cpu++) {
        kmt -> spin_lock(&task_lk);
        list_insert(&task_head, current_task[cpu]);
        kmt -> spin_unlock(&task_lk);
    }
    os -> on_irq(INT32_MIN, EVENT_NULL, kmt_save_context);
    os -> on_irq(INT32_MAX, EVENT_NULL, kmt_load_context);
    os -> on_irq(TIME_SEQ, EVENT_IRQ_TIMER, irq_time_handler);
    os -> on_irq(YIELD_SEQ, EVENT_YIELD, irq_yield_handler);


    //printf("kmt init finished");


}

static void list_add(task_t *head, task_t *task) {
    panic_on(!head, "wait list head is NULL");
    panic_on(!task, "the task added to wait_list is NULL");
    task_t *p = NULL;
    for (p = head; p -> sem_next != head; p = p -> sem_next);
    assert(p);
    p -> sem_next -> sem_prev = task;
    task -> sem_next = p -> sem_next;
    task -> sem_prev = p;
    p -> sem_next = task;

}

static task_t *list_delete(task_t **head) {
    panic_on(!(*head), "wait list head is NULL");
    task_t *p = NULL;
    (*head) -> sem_prev -> sem_next = (*head) -> sem_next;
    (*head) -> sem_next -> sem_prev = (*head) -> sem_prev;
    p = *head;
    *head = (*head) -> sem_next;

    return p;

}

static bool empty(task_t *head) {
    return head == NULL;
}
static void kmt_enqueue(task_t **head, task_t *task) {
    assert(task);
    if (!(*head)) {
        *head = task;
        assert(head);
    } else {
        list_add(*head, task);
    }
    
}

static task_t *kmt_dequeue(task_t **head) {
    panic_on(!(*head), "NULL queue");
    task_t *task = NULL;
    if ((*head) -> sem_next == *head) {
        task = *head; 
        *head = NULL;
    } else {
        task = list_delete(head);
    }
    return task;
}
static void kmt_sem_init(sem_t *sem, const char *name, int value) {
    assert(sem);
    strcpy(sem -> name, name);
    sem -> count = value;
    kmt -> spin_init(&(sem -> lock), name);
    sem -> wait_list = NULL;

}
static void kmt_sem_wait(sem_t *sem) {
    int count = 0;
    int cpu = cpu_current();
    task_t *task = current_task[cpu];
    kmt -> spin_lock(&(sem -> lock));
    sem -> count--;
    count = sem -> count;
    if (count < 0) {
        printf("++ %s\n", sem -> name);
        kmt_enqueue(&(sem -> wait_list), task);
        task -> status = WAIT_TO_WAKE_AND_SCHEDULE;
    }
    kmt -> spin_unlock(&(sem -> lock));
    if (count < 0) {
        yield();
    }

}
static void kmt_sem_signal(sem_t *sem) {
    int count = 0;
    kmt -> spin_lock(&(sem -> lock));
    sem -> count++;
    count = sem -> count;
    if (!empty(sem -> wait_list) && count >= 0) {

        printf("-- %s\n", sem -> name);
        task_t *task = kmt_dequeue(&(sem -> wait_list));
        printf("%p\n", task);
        if (task -> status == WAIT_TO_WAKE_AND_SCHEDULE) {
            task -> status = READY;
        } else {
            while (true) {
                if (task -> status == WAIT_TO_WAKE) {
                    task -> status = READY;
                    break;

                }
            }
        }

    }
    kmt -> spin_unlock(&(sem -> lock));
}

MODULE_DEF(kmt) = {
    .init = kmt_init,
    .spin_init = kmt_spin_init,
    .spin_lock = kmt_spin_lock,
    .spin_unlock = kmt_spin_unlock,
    .create = kmt_create,
    .sem_init = kmt_sem_init,
    .sem_wait = kmt_sem_wait,
    .sem_signal = kmt_sem_signal,
};
