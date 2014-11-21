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
