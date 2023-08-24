#include <os.h>

static void list_insert(task_t *head, task_t *task) {
    panic_on(atomic_xchg(&(task_lk.lock), KMT_LOCK) == KMT_UNLOCK, "error, the lock is not acquired");
    assert(head);
    assert(task);
    task_t *p = NULL;
    for (p = head; p -> next != head; p = p -> next);
    assert(p);
    p -> next -> prev = task;
    task -> next = p -> next;
    task -> prev = p;
    p -> next = task;


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
void idle_entry(void *arg) {
    while(1) {
        printf("Hello world from cpu %d\n", cpu_current());
    };
}

static void check_static_fence(task_t *task);

static void kmt_spin_init(spinlock_t *lk, const char *name) {
    strcpy(lk -> name, name);
    lk -> lock = KMT_UNLOCK;

}

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

        printf("hello world\n");
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
        yield();

        printf("is here!!!\n");
        kmt -> spin_lock(&task_lk);
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
            panic("error status");
        }

        task_t *p = task -> next;
        while (p != READY) {p = p -> next;}
        p -> round = KMT_INIT_ROUND;
        p -> status = WAIT_TO_LOAD;
        check_static_fence(p);
        

        current_task[cpu] = p;
        kmt -> spin_unlock(&task_lk);
    }
}



static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {
    if (!task || !entry) {
        return 1;
    }
    strcpy(task -> name, name);
    Area stack = (Area) {&task -> stack, &task -> stack + KMT_STACK_SIZE};
    task -> context = kcontext(stack, entry, arg);
    task -> round = KMT_INIT_ROUND;
    task -> status = READY;
    for (int i = 0; i < KMT_FENCE_SIZE; i++) {
        task -> fence1[i] = task -> fence2[i] = KMT_FENCE;
    }
    check_static_fence(task);
    kmt -> spin_lock(&task_lk);
    list_insert(current_task[0], task);
    kmt -> spin_unlock(&task_lk);
    return 0;
}


static void kmt_init() {
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
        task -> next = task -> prev = task;
        for (int i = 0; i < KMT_FENCE_SIZE; i++) {
            task -> fence1[i] = task -> fence2[i] = KMT_FENCE;
        }

        current_task[cpu] = task;
        check_static_fence(task);
    }

    for (int cpu = 1; cpu < cpu_count(); cpu++) {
        kmt -> spin_lock(&task_lk);
        list_insert(current_task[0], current_task[cpu]);
        kmt -> spin_unlock(&task_lk);
    }
    os -> on_irq(INT32_MIN, EVENT_NULL, kmt_save_context);
    os -> on_irq(INT32_MAX, EVENT_NULL, kmt_load_context);
    os -> on_irq(TIME_SEQ, EVENT_IRQ_TIMER, irq_time_handler);
    os -> on_irq(YIELD_SEQ, EVENT_YIELD, irq_yield_handler);


    printf("kmt init finished");




}
MODULE_DEF(kmt) = {
    .init = kmt_init,
    .spin_init = kmt_spin_init,
    .spin_lock = kmt_spin_lock,
    .spin_unlock = kmt_spin_unlock,
    .create = kmt_create,
};
