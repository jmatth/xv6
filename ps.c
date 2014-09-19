#include "types.h"
#include "user.h"
#include "param.h"
#include "proc.h"

int main(int argc, char *argv[])
{
  int i;
  struct proctable *myptable;
  myptable = malloc(sizeof(struct proctable));
  struct proc nproc;
  char *nstate;

  getproc(myptable);

  printf(1, "PID    STATE    NAME\n");

  for (i = 0; i < NPROC; ++i)
  {
    nproc = myptable->proc[i];
    switch (nproc.state) {
        case EMBRYO:
            nstate = "EMBRYO  ";
            break;
        case SLEEPING:
            nstate = "SLEEPING";
            break;
        case RUNNABLE:
            nstate = "RUNNABLE";
            break;
        case RUNNING:
            nstate = "RUNNING ";
            break;
        case ZOMBIE:
            nstate = "ZOMBIE  ";
            break;
        default:
            continue;
    }

    printf(1, "%d    %s    %s\n", nproc.pid,
                              nstate,
                              nproc.name);
  }


  exit();
}
