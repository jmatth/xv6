#include "user.h"

volatile int mutexmd;
volatile int excluded;

void *threadFunc(void* arg)
{
  int i, j;
  printf(1, "    in thread\n");

  for (i = 0; i < 100; ++i)
  {
    mutex_lock(mutexmd);
    excluded = 0;
    for (j = 0; j < 1000; ++j)
      excluded++;
    if (excluded != 1000)
    {
      printf(1, "!!!!!ERROR, EXCLUSION FAILED!!!!!!!");
      exit();
    }
    mutex_unlock(mutexmd);
  }

  printf(1, "    exiting thread\n");
  exit();
  return (void*)0xdeadbeef;
}

int main(int argc, char *argv[])
{
  int i, j;
  int thread_pid;

  printf(1, "in main thread\n");
  mutexmd = mutex_init();

  void *newstack = malloc(4096);

  thread_pid = clone((void*)&threadFunc, 0, newstack);

  printf(1, "cloned new thread\n");
  for (i = 0; i < 100; ++i)
  {
    mutex_lock(mutexmd);
    excluded = 0;
    for (j = 0; j < 1000; ++j)
      excluded++;
    if (excluded != 1000)
    {
      printf(1, "!!!!!ERROR, EXCLUSION FAILED!!!!!!!; %d\n", j);
      exit();
    }
    mutex_unlock(mutexmd);
  }

  printf(1, "joining in main\n");
  join(&newstack);
  printf(1, "done joining in main. exiting\n");

  exit();
  return 0;
}
