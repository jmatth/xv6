#include "types.h"
#include "stat.h"
#include "user.h"

int (*x)(void);

void dummy()
{
    return;
}

int exploitStage2() {
    return 2;
}

void* exploitStage1() {
    char *(*kalloc)(void);
    char *newpage;
    /* int oldcall; */

    kalloc = (char*(*)())findkalloc();
    newpage =kalloc();

    memmove(newpage, &exploitStage2, 1024);
    /* sysreplace(14, (uint)newpage, (uint)&oldcall); */

    return newpage;
}

int myuptime(void)
{
  int u = x();
  return u + 50000;
}

int main()
{
  int u;
  x = 0;

  u = uptime();
  printf(1, "%d\n", u);

  sysreplace(14, (uint) &exploitStage1, (uint) &x);
  u = uptime();
  sysreplace(14, (uint) u, (uint) &x);

  u = uptime();
  printf(1, "%d\n", u);

  exit();
}
