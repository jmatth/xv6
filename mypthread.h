typedef int mypthread_t;

typedef int mypthread_attr_t;

typedef int mypthread_mutex_t;

typedef int mypthread_mutex_attr_t;

typedef struct threadnode {
  int pid;
  void *retval;
  struct threadnode *next;
} threadnode;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
void mypthread_exit(void *retval);
int mypthread_join(mypthread_t thread, void **retval);
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);
int mypthread_mutex_init(mypthread_mutex_t *mutex, const mypthread_mutex_attr_t *attr);
int mypthread_mutex_lock(mypthread_mutex_t *mutex);
int mypthread_mutex_trylock(mypthread_mutex_t *mutex);
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);
int mypthread_yield();
