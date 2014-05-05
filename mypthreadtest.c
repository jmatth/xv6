#include "user.h"
#include "mypthread.c"

void* threadFunc(void* arg) {
  printf(1, "   in thread with pid %d. exiting with pthread_exit\n", getpid());
  mypthread_exit((void*)0xcafebabe);
  return 0;
}

int main(int argc, char *argv[])
{
  mypthread_t thread;
  void *retval;

  printf(1, "making thread\n");
  mypthread_create(&thread, 0, &threadFunc, (void*)0);
  printf(1, "thread created\n");

  printf(1, "joining on thread\n");
  mypthread_join(thread, &retval);
  printf(1, "After the join.\n");

  exit();
}
