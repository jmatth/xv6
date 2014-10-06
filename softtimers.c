#include "user.h"
#include "types.h"

int main(int argc, char *argv[])
{
  ushort data;
  data = microuptime();
  printf(1, "%d\n", data);
  exit();
}
