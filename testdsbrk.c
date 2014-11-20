#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *a = mallocd(14*4096);
  for(i=0; i < 14; i++)
  {
    *a = 'a';
    printf(1, "%d: %x is %c\n", i, a, *a);
    a += 4096;
  }
  exit();
}
