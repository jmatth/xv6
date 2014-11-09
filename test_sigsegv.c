#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

void handler(int a1, int a2)
{
  int marker;
  static int counter = 0;
  // Uncomment the next 3 lines to figure out the offset
  /* for(counter = 0; counter < 40; counter++) */
  /*   printf(1, "Mem with offeset %d is 0x%x\n", counter, *(int*)(&marker + counter)); */
  /* exit(); */

  // Uncomment this line to check that arguments work
  /* printf(1, "arg1: %d, arg2: %d\n", a1, a2); */

  switch (++counter) {
    case 1:
      printf(1, "In the handler the first time. Should segfault again after this.\n");
      break;
    case 2:
      printf(1, "In the handler a second time. Changing return address now.\n");
      *(int *)(&marker + 19) += 6;
      break;
    default:
      printf(1, "Didn't manage to change the return address. Something is broken\n");
      exit();
  }
}

int
main(int argc, char *argv[])
{
  signal(SIGSEGV, &handler);
  printf(1, "Dereferencing invalid memory...\n");
  *(int *)(-10) = 1;
  printf(1, "...crisis averted.\n");
  exit();
}

/* vim: set ts=2 sw=2 sts=2 expandtab: */
