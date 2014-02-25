#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if (argc >= 2)
  {
      incr(1);
  }

  printf(1, "Prev calls: %d\n", incr(0));
  exit();
}
