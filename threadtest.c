#include "types.h"
#include "user.h"
#include "mypthread.c"

void* threadFunc(void* arg) {
  printf(1, "Inside the thread.\n");
  mypthread_exit((void*)0xcafebabe);
  return 0;
}

int main(int argc, char *argv[])
{
  mypthread_t thread;
  void *retval;

  /* clone(&threadFunc, (void*)0, newStack); */
  mypthread_create(&thread, 0, &threadFunc, (void*)0);

  printf(1, "Hello world!\n");
  mypthread_join(thread, &retval);
  /* printf(1, "After the join.\n"); */
  exit();
}
