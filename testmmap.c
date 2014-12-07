#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"
#include "fcntl.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{
  char *mem;
  char fread[65];
  int fd;

  mem = sbrk(64 + PGSIZE);
  printf(1, "mem: 0x%x\n", mem);

  fd = open("README", O_RDWR);

  mmap((void*) mem, 64, PROT_READ | PROT_WRITE, 1, fd, 0);

  mem[64] = '\0';
  printf(1, mem);
  printf(1, "\n\n");

  memmove(mem, "JOSH", 4);

  read(fd, fread, 64);
  fread[64] = '\0';
  printf(1, fread);
  printf(1, "\n\n");

  exit();
}
