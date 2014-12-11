#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"
#include "fcntl.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{
  //char *mem, *mem2;
  char *mem;
  //char fread[65];
  int fd;
  int i;

  mem = sbrk(2*PGSIZE + PGSIZE);
  //mem2 = sbrk(PGSIZE + PGSIZE);
  printf(1, "mem: 0x%x\n", mem);
  //printf(1, "mem2: 0x%x\n", mem2);

  fd = open("README", O_RDWR);

  mmap((void *)mem, PGSIZE, PROT_WRITE, 1, fd, 0);
  for(i = 500; i < 600; i++) {
    mem[i] = 'J';
  }
  printf(1, "First char is %c\n", mem[0]);

  //if(fork() == 0) // In child
  //{
  //  mmap((void*) mem, 2*PGSIZE, PROT_READ, 1, fd, 0);
  //  sleep(100);
  //  printf(1, "mem1 char is '%d'\n", mem[PGSIZE]);
  //} else {
  //  sleep(50);
  //  mmap((void*) mem2, PGSIZE, PROT_WRITE, 1, fd, PGSIZE);
  //  printf(1, "mem2 char is '%d'\n", mem2[0]);
  //  wait();
  //}


  //mem2[5] = '\0';
  //printf(1, mem2);
  //printf(1, "\n\n");

  //memmove(mem, "JOSH", 4);

  //read(fd, fread, 64);
  //fread[64] = '\0';
  //printf(1, fread);
  //printf(1, "\n\n");

  close(fd);

  exit();
}
