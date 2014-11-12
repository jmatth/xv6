#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

void handler(int addr, int type)
{
  printf(1, "Addr is %x\n", addr);
  mprotect((void*)addr, 1, PROT_WRITE);
}

int
main(int argc, char *argv[])
{
  char *mem;
  signal(SIGSEGV, &handler);
  mem = malloc(5000);
  printf(1, "Mem is %x\n", mem);
  mprotect((void *)mem, 5000, PROT_READ);
  *mem = 'b';
  *(mem + 4500) = 'a';
  printf(1, "mem is %c and mem + 4500 is %c\n", *mem, *(mem+4500));
  exit();
}
