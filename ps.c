#include "types.h"
#include "user.h"
#include "param.h"
#include "proc.h"

int main(int argc, char *argv[])
{
  int i;
  struct proctable *myptable;
  myptable = malloc(sizeof(struct proctable));

  getproc(myptable);

  printf(1, "PID\tSTATE\tNAME\n");

  for (i = 0; i < NPROC; ++i)
  {
    printf(1, "%d\t%d\t%s\n", myptable->proc[i].pid,
                              myptable->proc[i].state,
                              myptable->proc[i].name);
  }


  exit();
}
