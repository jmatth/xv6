#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

int *mem;

void handler()
{
  mprotect((void*)mem, 1, PROT_WRITE);
}

int
main(int argc, char *argv[])
{
  signal(SIGSEGV, &handler);
  mem = malloc(sizeof(int));
  mprotect((void *)mem, 1, PROT_READ);
  *mem = 5;
  printf(1, "%d\n", *mem);
  exit();
}
