#include "context.h"

#ifndef MYPTHREAD_H
#define MYPTHREAD_H 1

#define STACKSIZE 16384

#define EBUSY 1

enum _pthread_state {
    RUNNING,
    SLEEPING,
    RUNNABLE,
    ZOMBIE,
    DEAD
};

typedef enum _pthread_state pthread_state;

struct threadnode {
    struct threadnode *next;
    int tid;
};

typedef struct _threadqueue {
    struct threadnode *front;
    struct threadnode *end;
} threadqueue;

struct pthread_cont {
    int tid;
    pthread_state state;
    ucontext_t context;
    char stack[STACKSIZE];
    void *retval;
    int sleeping_on;
    short int in_pthreads;
};

struct pthread_mutex {
    short int locked;
    long int id;
    int locked_by;
    threadqueue sleeping_on;
};


/**********************
 *  Internal helpers  *
 **********************/


typedef struct pthread_cont pthread_cont_t;

typedef struct pthread_mutex pthread_mutex_t;

typedef int pthread_mutexattr_t;

typedef int pthread_t;

void sched();

void init_timer();

inline void init_main_thread();

inline void init_thread(pthread_cont_t*, int);

int myenqueue(pthread_t, threadqueue*);

void myqueueinit(threadqueue*);

pthread_t mydequeue(threadqueue*);

void myqueueempty(threadqueue*);

/*****************
 *  Cooperative  *
 *****************/

void pthread_create(pthread_t *, const char *, void* (*)(void*), void*);

void pthread_yield();

int pthread_join(pthread_t, void**);

void pthread_exit();

/*********************
 *  Synchronization  *
 *********************/
void pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);

int pthread_mutex_lock(pthread_mutex_t*);

int pthread_mutex_unlock(pthread_mutex_t*);

int pthread_mutex_trylock(pthread_mutex_t*);

int pthread_mutex_destroy(pthread_mutex_t*);

#endif
