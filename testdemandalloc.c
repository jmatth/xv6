#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int ITER = 100;
  char *mem[ITER];
  int i,j;
  int start_alloc, end_alloc;
  int start_write, end_write;
  int dstart_alloc, dend_alloc;
  int dstart_write, dend_write;
  printf(1, "Running for %d allocs and writes each...\n", ITER);

  start_alloc = uptime();
  for(i=0; i<ITER; i++)
  {
    mem[i] = sbrk(4096);
  }
  end_alloc = uptime();

  start_write = uptime();
  for(i=0; i<ITER; i++)
  {
    for(j=0; j<4096; j++)
    {
      mem[i][j]='a';
    }
  }
  end_write = uptime();

  dstart_alloc = uptime();
  for(i=0; i<ITER; i++)
  {
    mem[i] = dsbrk(4096);
  }
  dend_alloc = uptime();

  dstart_write = uptime();
  for(i=0; i<100; i++)
  {
    for(j=0; j<4096; j++)
    {
      mem[i][j]='b';
    }
  }
  dend_write = uptime();

  printf(1, "Regular sbrk: Alloc took %d, Write took %d\n", end_alloc-start_alloc, end_write-start_write);
  printf(1, "Demand sbrk: Alloc took %d, Write took %d\n", dend_alloc-dstart_alloc, dend_write-start_write);

  exit();
}
