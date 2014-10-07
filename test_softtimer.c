#include "user.h"
#include "signal.h"

int start;

void handler1()
{
  int tmp = microuptime();
  printf(1, "Elasped time is %d\n", tmp - start);
  start = tmp;
  alarm(100000);
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, &handler1);
  start = microuptime();
  alarm(100000);

  while(1)
  {
    sleep(0);
    int i;
    for(i=0; i<50; ++i)
    {
      continue;
    }
  }
}
