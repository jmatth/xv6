#include "types.h"
#include "stat.h"
#include "user.h"

int (*x)(void);

void dummy()
{
    return;
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
  sysreplace(14, (uint) &myuptime, (uint) &x);
  u = uptime();
  printf(1, "%d\n", u);
  exit();
}
