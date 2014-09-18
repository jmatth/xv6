#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
  int *x = 0;
  sysreplace(0, 0, (uint)x);
  printf(1, "%d\n", *x);
  exit();
}
