#include "mypthread.h"
#include "types.h"
#include "user.h"

threadnode *deadThreads = 0;
int firstcall = 1;
int libMutex;

inline void libinit()
{
  libMutex = mutex_init();
  firstcall = 0;
}

void insertNode(threadnode *newNode)
{
  newNode->next = deadThreads;
  deadThreads = newNode;
}

threadnode* getNode(int pid)
{
  threadnode *retThread;
  threadnode *prev;

  if (deadThreads == 0){
    return 0;
  }

  if (deadThreads->pid == pid)
  {
    retThread = deadThreads;
    deadThreads = deadThreads->next;
    return retThread;
  }

  prev = deadThreads;
  retThread = prev->next;
  while(retThread != 0) {
    if (retThread->pid == pid)
    {
      prev->next = retThread->next;
      return retThread;
    }
    prev = retThread;
    retThread = retThread->next;
  }

  return 0;
}

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg)
{
  if (firstcall)
    libinit();

  void* new_stack = malloc(4096);
  *thread = clone((void*)start_routine, arg, new_stack);
  //FIXME: check if clone encountered an error?
  return 0;
}

void mypthread_exit(void *retval)
{
  mutex_lock(libMutex);

  threadnode *deadNode = malloc(sizeof(threadnode));
  deadNode->pid = getpid();
  deadNode->retval = retval;
  insertNode(deadNode);

  mutex_unlock(libMutex);
  exit();
}

int mypthread_yield()
{
  //FIXME maybe
  return 0;
}

int mypthread_join(mypthread_t thread, void **retval)
{
  threadnode *node;
  void* oldStack;
  int joinedPid;

  mutex_lock(libMutex);

  node = getNode(thread);
  if (node != 0)
    goto foundjoinnode;

  mutex_unlock(libMutex);
  while((joinedPid = join(&oldStack)) != thread)
  {
    free(oldStack);
    mutex_unlock(libMutex);
    mutex_lock(libMutex);
  }
  free(oldStack);
  mutex_lock(libMutex);

  node = getNode(thread);
  if (node == 0)
  {
    mutex_unlock(libMutex);
    return -1;
  }

foundjoinnode:
  *retval = node->retval;
  free(node);
  mutex_unlock(libMutex);
  return 0;
}
