#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;
  int pid = cowfork();
  if(pid == 0)
  {
    i = 5;
  } else {
    i = 3;
    exit();
  }
  printf(1, "I is %d\n", i);
  exit();
}
