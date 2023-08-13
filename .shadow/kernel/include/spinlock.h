#ifndef HEADER_FILE_SPINLOCK  
#define HEADER_FILE_SPINLOCK  
#ifndef TEST
#include <klib.h>
#include <am.h>
typedef int spinlock_t;
#define SPIN_INIT() 0
#define LOCKED 1
#define UNLOCKED 0
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
uintptr_t try_lock(spinlock_t *lk); 
#else
#define SPIN_INIT() PTHREAD_MUTEX_INITIALIZER
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
#define LOCKED 1
#define UNLOCKED 0

typedef pthread_mutex_t spinlock_t;

void spin_lock(spinlock_t *lk);   
void spin_unlock(spinlock_t *lk); 

#endif
#endif