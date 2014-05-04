#include "mypthread.h"
#include "types.h"
#include "user.h"

threadnode *deadThreads = 0;

void insertNode(threadnode *newNode)
{
  newNode->next = deadThreads;
  deadThreads = newNode;
}

threadnode* getNode(int pid)
{
  threadnode *retThread;
  threadnode *prev;

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
  void* new_stack = malloc(4096);
  clone((void*)start_routine, arg, new_stack);
  //FIXME: check if clone encountered an error?
  return 0;
}

void mypthread_exit(void *retval)
{
  threadnode *deadNode = malloc(sizeof(threadnode));
  deadNode->pid = getpid();
  deadNode->retval = retval;
  insertNode(deadNode);
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

  node = getNode(thread);
  if (node != 0)
  {
    *retval = node->retval;
    free(node);
    return 0;
  }

  while(join(&oldStack) != thread);

  node = getNode(thread);
  if (node == 0)
    return -1;

  *retval = node->retval;
  free(node);
  return 0;
}
