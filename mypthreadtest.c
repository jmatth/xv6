#include "user.h"
#include "mypthread.h"

void* threadFunc(void* arg) {
  printf(1, "   in thread with pid %d. exiting with pthread_exit\n", getpid());
  pthread_exit((void*)0xcafebabe);
  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t thread;
  void *retval;

  printf(1, "making thread\n");
  pthread_create(&thread, 0, &threadFunc, (void*)0);
  printf(1, "thread created\n");

  printf(1, "joining on thread\n");
  pthread_join(thread, &retval);
  printf(1, "After the join.\n");

  exit();
}
