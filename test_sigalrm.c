#include "user.h"
#include "signal.h"

void handler1()
{
  printf(1, "ring ring\n");
}

void handler2()
{
  printf(1, "SUCCESS: user space spinning interupted\n");
  exit();
}

int main(int argc, char *argv[])
{
  int val;

  signal(SIGALRM, &handler1);
  alarm(100);
  val = sleep(10000000);
  if(val <= 0)
  {
    printf(1, "ERROR: sleep not interupted\n");
    exit();
  }
  printf(1, "SUCCESS: sleep interupted\n");

  signal(SIGALRM, &handler2);
  alarm(100);
  val = 0;
  while(1)
    val++;

  exit();
}
