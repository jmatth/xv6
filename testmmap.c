#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"
#include "fcntl.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{
  char *mem, *mem1, *mem2;
  int fd, bigfd;
  int i;

  mem = sbrk(PGSIZE + PGSIZE);
  mem1 = sbrk(2*PGSIZE + PGSIZE);
  mem2 = sbrk(PGSIZE + PGSIZE);
  //printf(1, "mem2: 0x%x\n", mem2);

  fd = open("README", O_RDWR);
  bigfd = open("BIGFILE", O_RDWR);

  printf(1,"mmaping first page of README, growing sz to PGSIZE\n");
  mmap((void *)mem, PGSIZE, PROT_WRITE, 1, fd, 0);
  printf(1, "Making bytes 500-599 Js\n");
  for(i = 500; i < 600; i++) {
    mem[i] = 'J';
  }

  char readmem[101], garbage[500];
  printf(1, "Writing first 5 chars to be KATIE\n");
  write(fd, "KATIE", 5);
  printf(1, "Reading in first 500 chars to advance cursor\n");
  read(fd, garbage, 495);
  printf(1, "Reading in next 100 chars, which should now be Js.\n");
  read(fd, readmem, 100);
  readmem[100] = 0;
  printf(1, "Chars: %s\n", readmem);

  printf(1, "The first 5 chars at mmaped region are %c%c%c%c%c\n", mem[0], mem[1], mem[2], mem[3], mem[4]);

  printf(1, "Closing README fd\n");
  close(fd);

  printf(1, "About to fork to test multiple processes\n");
  if(fork() == 0) // In child
  {
    printf(1, "mmaping first 2 pages of BIGFILE read only\n");
    mmap((void*) mem1, 2*PGSIZE, PROT_READ, 0, bigfd, 0);
    sleep(100);
    printf(1, "In other process, first char on 2nd page is '%c'\n", mem1[PGSIZE]);
    //printf(1, "About to write to read only portion, should seg fault\n");
    //mem1[0] = 'x';
    close(bigfd);
    exit();
  } else {
    sleep(50);
    printf(1, "Mapping 2nd page of BIGFILE writable\n");
    mmap((void*) mem2, PGSIZE, PROT_WRITE, 0, bigfd, PGSIZE);
    printf(1, "Setting first char of 2nd page to f\n");
    mem2[0] = 'f';
    wait();
    close(bigfd);
  }

  printf(1, "Closing BIGFILE fd\n");

  printf(1, "Cat README to see changes. Size should be 4096\n");

  exit();
}
