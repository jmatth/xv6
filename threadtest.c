#include "types.h"
#include "user.h"
#include "mypthread.h"

void* threadFunc(void* arg) {
  printf(1, "Inside the thread.\n");
  pthread_exit((void*)0xcafebabe);
  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t thread;
  void *retval;

  /* clone(&threadFunc, (void*)0, newStack); */
  pthread_create(&thread, 0, &threadFunc, (void*)0);

  printf(1, "Hello world!\n");
  pthread_join(thread, &retval);
  /* printf(1, "After the join.\n"); */
  exit();
}
