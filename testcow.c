#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  printf(1, "i is at 0x%x\npid is at 0x%x\n", &i, &pid);
  pid = cowfork();
  i = 3;
  if(pid == 0)
  {
    i = 5;
    printf(1, "I think i'm in the child, and i is %d\n", i);
    exit();
  }

  wait();
  printf(1, "I think i'm in the parent and i is %d\n", i);
  exit();
}
