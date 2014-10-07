#include "user.h"
#include "signal.h"

void handler1()
{
  printf(1, "ring ring\n");
  exit();
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, &handler1);
  alarm(10500);

  while(1)
  {
    printf(1, "%d\n", microuptime());
  }
}
