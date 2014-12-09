#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"
#include "fcntl.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{
  char *mem, *mem2;
  //char fread[65];
  int fd;

  mem = sbrk(2*PGSIZE + PGSIZE);
  mem2 = sbrk(PGSIZE + PGSIZE);
  printf(1, "mem: 0x%x\n", mem);
  printf(1, "mem2: 0x%x\n", mem2);

  fd = open("README", O_RDWR);

  mmap((void*) mem, 2*PGSIZE, PROT_READ | PROT_WRITE, 1, fd, 0);
  mmap((void*) mem2, PGSIZE, PROT_READ | PROT_WRITE, 1, fd, PGSIZE);
  printf(1, "mem2 char is '%d'\n", mem2[0]);

  mem2[5] = '\0';
  printf(1, mem2);
  printf(1, "\n\n");

  memmove(mem, "JOSH", 4);

  //read(fd, fread, 64);
  //fread[64] = '\0';
  //printf(1, fread);
  //printf(1, "\n\n");

  close(fd);

  exit();
}
