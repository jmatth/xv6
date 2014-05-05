typedef int mypthread_t;

typedef int mypthread_attr_t;

typedef int mypthread_mutex_t;

typedef int mypthread_mutex_attr_t;

typedef struct threadnode {
  int pid;
  void *retval;
  struct threadnode *next;
} threadnode;
