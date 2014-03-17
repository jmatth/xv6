#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

void handler()
{
  int marker;
  static int counter = 0;
  switch (++counter) {
    case 1:
      printf(1, "In the handler the first time. Should segfault again after this.\n");
      break;
    case 2:
      printf(1, "In the handler a second time. Changing return address now.\n");
      *(int *)(&marker + 15) += 6;
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
