#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

#define ITERS 100000

void handler()
{
  int marker;
  static int counter = 0;
  if (++counter > ITERS)
    *(int *)(&marker + 13) += 6;
}

int
main(int argc, char *argv[])
{
  int before, after, diff;
  signal(SIGSEGV, &handler);
  before = uptime();
  *(int *)(-10) = 1;
  after = uptime();
  diff = after - before;
  printf(1, "Took %d ticks for %d signals.\n", diff, ITERS);
  exit();
}

/* vim: set ts=2 sw=2 sts=2 expandtab: */
