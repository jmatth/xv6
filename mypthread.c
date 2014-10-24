#include "user.h"
#include "signal.h"
#include "context.h"
#include "mypthread.h"

#define CLOCKTICKSEC  0
#define CLOCKTICKUSEC 5

#define NULL 0

int thread_table_l;
pthread_cont_t **thread_table;
int curr_thread;

long int next_mutex_id = 0;

threadqueue mainqueue;

short int firstcall = 1;

#define LOCKLIB thread_table[curr_thread]->in_pthreads = 1
#define UNLOCKLIB thread_table[curr_thread]->in_pthreads = 0

void exit_cleanup(){
    LOCKLIB;

    int i;

    if (firstcall)
        return;

    for (i = 0; i < thread_table_l; i++)
    {
        if (thread_table[i] == NULL)
            continue;

        free(thread_table[i]);
    }

    free(thread_table);

    myqueueempty(&mainqueue);
}

void timer_handler(int sig)
{
    if (!thread_table[curr_thread]->in_pthreads)
    {
        LOCKLIB;
        sched(RUNNABLE);
    }

    UNLOCKLIB;
}

inline void swtch(pthread_t next, pthread_state state)
{
    pthread_cont_t *this_thread = thread_table[curr_thread];

    if (state == RUNNABLE)
        myenqueue(curr_thread, &mainqueue);

    this_thread->state = state;
    thread_table[next]->state = RUNNING;
    curr_thread = next;

    swapcontext(&(this_thread->context), &(thread_table[next]->context));
}

inline void sched(pthread_state state)
{
    int next_tid;

    next_tid = mydequeue(&mainqueue);

    if(next_tid == -1)
        return;

    swtch(next_tid, state);
}

inline void init_main_thread()
{
    thread_table = malloc(sizeof(pthread_cont_t*) * 256);
    memset(thread_table, 0, sizeof(pthread_cont_t*) * 256);
    thread_table_l = 256;
    curr_thread = 0;
    thread_table[curr_thread] = malloc(sizeof(pthread_cont_t));

    init_thread(thread_table[curr_thread], curr_thread);

    LOCKLIB;

    /* thread_table[curr_thread]->context.uc_link = NULL; */
    thread_table[curr_thread]->state = RUNNING;

    myqueueinit(&mainqueue);

    firstcall = 0;
    signal(SIGALRM, timer_handler);
    /* atexit(exit_cleanup); */

    init_timer();
}

inline void init_thread(pthread_cont_t *thread, int tid)
{
    getcontext(&(thread->context));
    thread->tid = tid;
    thread->context.uc_stack.ss_sp = thread->stack;
    thread->context.uc_stack.ss_size = STACKSIZE;
    /* thread->context.uc_link = &(thread_table[curr_thread]->context); */
    thread->state = RUNNABLE;
    thread->in_pthreads = 0;
    thread->sleeping_on = -1;
}

void init_timer()
{
    alarm(CLOCKTICKUSEC);
}

/**********************************
 *  Cooperative thread functions  *
 **********************************/

void pthread_create(pthread_t *thread, const char *unused,
                     void* (*func)(void*), void *arg)
{
    int i, next_tid;

    if (firstcall)
        init_main_thread();

    LOCKLIB;

    for (i = 0; i < thread_table_l; ++i)
    {
        if (thread_table[i] == NULL)
        {
            next_tid = i;
            thread_table[next_tid] = malloc(sizeof(pthread_cont_t));
            break;
        }
        else if (thread_table[i]->state == DEAD)
        {
            next_tid = i;
            break;
        }
    }

    init_thread(thread_table[next_tid], next_tid);

    *thread = next_tid;

    makecontext(&(thread_table[next_tid]->context), (void*)func, 1, (int)arg);

    myenqueue(next_tid, &mainqueue);
    UNLOCKLIB;
}

void  pthread_yield()
{
    LOCKLIB;
    sched(RUNNABLE);
    UNLOCKLIB;
}

void pthread_exit(void *retval)
{
    LOCKLIB;
    thread_table[curr_thread]->retval = retval;

    if (thread_table[curr_thread]->sleeping_on != -1)
        myenqueue(thread_table[curr_thread]->sleeping_on, &mainqueue);

    sched(ZOMBIE);
    UNLOCKLIB;
}

int pthread_join(pthread_t thread, void **retval)
{
    LOCKLIB;
    pthread_cont_t *real_thread = thread_table[thread];

    if (real_thread->state == ZOMBIE)
    {
        real_thread->state = DEAD;
        if (retval != NULL)
            *retval = real_thread->retval;
        UNLOCKLIB;
        return 0;
    }

    real_thread->sleeping_on = curr_thread;

    sched(SLEEPING);

    real_thread->state = DEAD;
    if (retval != NULL)
        *retval = real_thread->retval;

    UNLOCKLIB;
    return 0;
}


/*********************
 *  Synchronization  *
 *********************/
void pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
    if (firstcall)
        init_main_thread();

    LOCKLIB;

    mutex->locked = 0;
    mutex->id = next_mutex_id++;
    mutex->locked_by = -1;
    myqueueinit(&(mutex->sleeping_on));

    UNLOCKLIB;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    LOCKLIB;

    while (mutex->locked)
    {
        myenqueue(curr_thread, &(mutex->sleeping_on));
        sched(SLEEPING);
    }

    mutex->locked = 1;
    mutex->locked_by = curr_thread;

    UNLOCKLIB;
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    LOCKLIB;

    int next_tid;

    if (mutex->locked_by != curr_thread)
    {
        UNLOCKLIB;
        return 1;
    }

    mutex->locked = 0;
    mutex->locked_by = -1;

    next_tid = mydequeue(&(mutex->sleeping_on));

    if (next_tid != -1)
    {
        swtch(next_tid, RUNNABLE);
    }
    else
    {
        sched(RUNNABLE);
    }

    UNLOCKLIB;

    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    LOCKLIB;

    if (mutex->locked)
    {
        UNLOCKLIB;
        return EBUSY;
    }

    mutex->locked = 1;

    UNLOCKLIB;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    myqueueempty(&(mutex->sleeping_on));
    return 0;
}


/**************************
 *  Internal queue stuff  *
 **************************/

void myqueueinit(threadqueue *queue)
{
    queue->front = NULL;
    queue->end = NULL;
}

void myqueueempty(threadqueue *queue)
{
    while(mydequeue(queue) != -1);
}

int myenqueue(pthread_t thread, threadqueue *queue)
{
    struct threadnode *node;

    node = malloc(sizeof(struct threadnode));
    node->tid = thread;
    node->next = NULL;

    if (queue->end == NULL)
    {
        queue->end = node;
        queue->front = node;
    }
    else
    {
        queue->end->next = node;
        queue->end = node;
    }

    return 0;
}

pthread_t mydequeue(threadqueue *queue)
{
    pthread_t ret;
    struct threadnode *curr_front;

    if (queue->front == NULL)
        return -1;

    curr_front = queue->front;

    ret = curr_front->tid;

    queue->front = curr_front->next;
    if (queue->front == NULL)
        queue->end = NULL;

    free(curr_front);

    return ret;
}
