#include "user.h"
#include "signal.h"

void handler()
{
  printf(1, "ring ring\n");
  alarm(100);
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, &handler);
  alarm(100);

  while(1)
  {
    printf(1, "%d\n", sleep(10000));
  }
}
