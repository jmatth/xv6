#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;
  int pid;
  printf(1, "i is at 0x%x; pid is at 0x%x\n", &i, &pid);
  pid = cowfork();
  printf(1, "pid: %d\n", pid);
  i = 3;
  if(pid == 0)
  {
    i = 5;
    printf(1, "I think i'm in the child, and i is %d\n", i);
    while(1)
      sleep(100);
  }

  printf(1, "I think i'm in the parent and i is %d\n", i);
  while(1)
    sleep(100);
  exit();
}
