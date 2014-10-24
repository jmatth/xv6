#include "types.h"
#include "user.h"
#include "mypthread.h"

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

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg)
{
  if (firstcall)
    libinit();

  void* new_stack = malloc(4096);
  *thread = clone((void*)start_routine, arg, new_stack);
  //FIXME: check if clone encountered an error?
  return 0;
}

void pthread_exit(void *retval)
{
  mutex_lock(libMutex);

  threadnode *deadNode = malloc(sizeof(threadnode));
  deadNode->pid = getpid();
  deadNode->retval = retval;
  insertNode(deadNode);

  mutex_unlock(libMutex);
  exit();
}

int pthread_yield()
{
  yield();
  return 0;
}

int pthread_join(pthread_t thread, void **retval)
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
    if(joinedPid != -1)
      free(oldStack);
    mutex_unlock(libMutex);
    if ((node = getNode(thread)) != 0)
      break;
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

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutex_attr_t *attr)
{
  *mutex = mutex_init();
  return *mutex;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
  return mutex_lock(*mutex);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
  return mutex_trylock(*mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
  int retval;
  retval = mutex_unlock(*mutex);
  pthread_yield();
  return retval;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
  return mutex_destroy(*mutex);
}
